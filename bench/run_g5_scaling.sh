#!/usr/bin/env bash
# Gate 5 scaling driver. Implements docs/g5_scaling_plan.md exactly:
#   Suites A-D: 1 unrecorded warmup + 7 recorded runs per configuration.
#   Suite E   : 10 interleaved overlap/non-overlap pairs, balanced lead order.
# All runs are bound to physical P-cores (logical CPUs 0,2,4,6) with no SMT sharing.
#
# Usage: bench/run_g5_scaling.sh [A] [B] [C] [D] [E]     (default: all suites)
#        G5_REPS=3 bench/run_g5_scaling.sh A              (override recorded reps for a dry run)
#        G5_OUT=results/dryrun bench/run_g5_scaling.sh A  (override output directory)
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${REPO_ROOT}"

BIN="${REPO_ROOT}/build/release/heat_solver"
REPS="${G5_REPS:-7}"
PAIRS="${G5_PAIRS:-10}"
OUT="${G5_OUT:-${REPO_ROOT}/results/g5}"
mkdir -p "${OUT}"

export OMP_PLACES=cores
export OMP_PROC_BIND=close

cpus_for() {  # total cores -> physical P-core logical CPU list
    case "$1" in
        1) echo "0" ;;
        2) echo "0,2" ;;
        4) echo "0,2,4,6" ;;
        *) echo "unsupported core count $1" >&2; exit 1 ;;
    esac
}

mean_mhz() {  # mean MHz across the CPUs in list $1
    local list="$1"
    awk -v list="${list}" 'BEGIN{n=split(list,a,","); for(i=1;i<=n;i++) want[a[i]]=1}
        /^processor/ {p=$3} /^cpu MHz/ { if (p in want) {s+=$4; c++} }
        END { if (c) printf "%.0f", s/c; else printf "NA" }' /proc/cpuinfo
}

TELEMETRY="${OUT}/telemetry.csv"
[[ -f "${TELEMETRY}" ]] || echo "tag,phase,loadavg1,mean_mhz,epoch" > "${TELEMETRY}"

# run_one <csv> <tag> <dim> <nx> <ny> <nz> <steps> <P> <T> <overlap> <bz> <by> <record:0|1>
run_one() {
    local csv="$1" tag="$2" dim="$3" nx="$4" ny="$5" nz="$6" steps="$7" P="$8" T="$9"
    local overlap="${10}" bz="${11}" by="${12}" record="${13}"
    local cores=$((P * T))
    local cpus; cpus="$(cpus_for "${cores}")"
    local args=(--dim="${dim}" --nx="${nx}" --ny="${ny}" --steps="${steps}" --dt=auto
                --overlap="${overlap}")
    [[ "${dim}" == "3" ]] && args+=(--nz="${nz}" --bz="${bz}" --by="${by}")
    export OMP_NUM_THREADS="${T}"
    if [[ "${record}" == "1" ]]; then
        export BENCHMARK_CSV_FILE="${csv}"
        export BENCHMARK_TAG="${tag}"
        echo "${tag},before,$(cut -d' ' -f1 /proc/loadavg),$(mean_mhz "${cpus}"),$(date +%s)" >> "${TELEMETRY}"
    else
        unset BENCHMARK_CSV_FILE BENCHMARK_TAG
    fi
    taskset -c "${cpus}" mpirun --map-by "slot:PE=${T}" --bind-to core -np "${P}" \
        "${BIN}" "${args[@]}" > /dev/null
    if [[ "${record}" == "1" ]]; then
        echo "${tag},after,$(cut -d' ' -f1 /proc/loadavg),$(mean_mhz "${cpus}"),$(date +%s)" >> "${TELEMETRY}"
    fi
}

# scaling_point <suite> <label> <dim> <nx> <ny> <nz> <steps> <P> <T> <bz> <by>
scaling_point() {
    local suite="$1" label="$2" dim="$3" nx="$4" ny="$5" nz="$6" steps="$7" P="$8" T="$9"
    local bz="${10}" by="${11}"
    local csv="${OUT}/g5_suite_${suite}.csv"
    echo "[${suite}] ${label}: ${dim}D ${nx}x${ny}x${nz} steps=${steps} P=${P} T=${T}"
    run_one "${csv}" "" "${dim}" "${nx}" "${ny}" "${nz}" "${steps}" "${P}" "${T}" true "${bz}" "${by}" 0
    for rep in $(seq 1 "${REPS}"); do
        run_one "${csv}" "${suite};${label};${rep}" "${dim}" "${nx}" "${ny}" "${nz}" "${steps}" \
            "${P}" "${T}" true "${bz}" "${by}" 1
    done
}

strong_suite() {  # <suite> <dim> <n> <steps> <bz> <by>
    local suite="$1" dim="$2" n="$3" steps="$4" bz="$5" by="$6"
    local nz=1; [[ "${dim}" == "3" ]] && nz="${n}"
    rm -f "${OUT}/g5_suite_${suite}.csv"
    scaling_point "${suite}" "mpi_P1_T1"    "${dim}" "${n}" "${n}" "${nz}" "${steps}" 1 1 "${bz}" "${by}"
    scaling_point "${suite}" "mpi_P2_T1"    "${dim}" "${n}" "${n}" "${nz}" "${steps}" 2 1 "${bz}" "${by}"
    scaling_point "${suite}" "mpi_P4_T1"    "${dim}" "${n}" "${n}" "${nz}" "${steps}" 4 1 "${bz}" "${by}"
    scaling_point "${suite}" "omp_P1_T2"    "${dim}" "${n}" "${n}" "${nz}" "${steps}" 1 2 "${bz}" "${by}"
    scaling_point "${suite}" "omp_P1_T4"    "${dim}" "${n}" "${n}" "${nz}" "${steps}" 1 4 "${bz}" "${by}"
    scaling_point "${suite}" "hybrid_P2_T2" "${dim}" "${n}" "${n}" "${nz}" "${steps}" 2 2 "${bz}" "${by}"
}

overlap_suite() {  # <suite> <dim> <n> <steps> <bz> <by>
    local suite="$1" dim="$2" n="$3" steps="$4" bz="$5" by="$6"
    local nz=1; [[ "${dim}" == "3" ]] && nz="${n}"
    local csv="${OUT}/g5_suite_${suite}.csv"
    rm -f "${csv}"
    echo "[${suite}] overlap evaluation: ${dim}D ${n}, P=4, ${PAIRS} interleaved pairs"
    run_one "${csv}" "" "${dim}" "${n}" "${n}" "${nz}" "${steps}" 4 1 true  "${bz}" "${by}" 0
    run_one "${csv}" "" "${dim}" "${n}" "${n}" "${nz}" "${steps}" 4 1 false "${bz}" "${by}" 0
    for pair in $(seq 1 "${PAIRS}"); do
        # odd pairs: overlap first; even pairs: non-overlap first (balanced lead order)
        if (( pair % 2 == 1 )); then first=true; second=false; lead="ovfirst"; else first=false; second=true; lead="novfirst"; fi
        for mode in "${first}" "${second}"; do
            run_one "${csv}" "${suite};overlap_${mode};${pair};${lead}" "${dim}" "${n}" "${n}" "${nz}" \
                "${steps}" 4 1 "${mode}" "${bz}" "${by}" 1
        done
    done
}

weak_suite() {  # <suite> <dim> <steps> <bz> <by> then triples "nx ny nz P" for P=1,2,4
    local suite="$1" dim="$2" steps="$3" bz="$4" by="$5"; shift 5
    rm -f "${OUT}/g5_suite_${suite}.csv"
    while (( $# >= 4 )); do
        scaling_point "${suite}" "weak_P$4" "${dim}" "$1" "$2" "$3" "${steps}" "$4" 1 "${bz}" "${by}"
        shift 4
    done
}

SUITES=("$@")
[[ ${#SUITES[@]} -eq 0 ]] && SUITES=(A B C D E)

echo "G5 start: $(date -Is)  reps=${REPS} pairs=${PAIRS} out=${OUT}  load=$(cut -d' ' -f1-3 /proc/loadavg)"
for s in "${SUITES[@]}"; do
    case "${s}" in
        A) strong_suite A 2 4096 100 0 0 ;;
        B) strong_suite B 3 512 50 32 32 ;;
        C) weak_suite C 2 100 0 0   2048 2048 1 1   2048 4096 1 2   4096 4096 1 4 ;;
        D) weak_suite D 3 30 32 32  256 256 256 1   256 256 512 2   256 512 512 4 ;;
        E) overlap_suite E2D 2 4096 100 0 0
           overlap_suite E3D 3 512 50 32 32 ;;
        *) echo "unknown suite ${s}" >&2; exit 1 ;;
    esac
done
echo "G5 done: $(date -Is)"
