#!/usr/bin/env bash
export OMP_PLACES="{0},{2},{4},{6}"
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=4

/mnt/c/University/PDC/Project/build/release/heat_solver --config /mnt/c/University/PDC/Project/configs/large_2d.cfg &
SOLVER_PID=$!
sleep 0.8

echo "=== INSPECTING PROCESS ${SOLVER_PID} THREADS ==="
for t_dir in /proc/${SOLVER_PID}/task/*; do
    tid=$(basename "${t_dir}")
    cpu=$(awk '{print $39}' "${t_dir}/stat")
    core=$(cat /sys/devices/system/cpu/cpu${cpu}/topology/core_id 2>/dev/null || echo "?")
    echo "TID: ${tid} -> Running on CPU (PSR): ${cpu} (physical core_id: ${core})"
done

wait ${SOLVER_PID}
