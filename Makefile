# Top-Level Build and Orchestration Makefile
# Parallel Heat Diffusion Solver (MPI + OpenMP)

.PHONY: all build debug test test-mpi demo preflight clean format-check help

PYTHON ?= $(shell command -v python3 || command -v python)

all: build

help:
	@echo "================================================================="
	@echo " Parallel Heat Diffusion Simulation Build & Orchestration Engine"
	@echo "================================================================="
	@echo " Available Targets:"
	@echo "   make build        : Configure and build optimized Release preset"
	@echo "   make debug        : Configure and build Debug preset"
	@echo "   make test         : Execute complete GoogleTest test suite via CTest"
	@echo "   make preflight    : Run HPC runtime environment and affinity check"
	@echo "   make demo         : Run full verification, scaling, and I/O demo"
	@echo "   make format-check : Verify code formatting against .clang-format"
	@echo "   make clean        : Remove build trees and temporary run artifacts"
	@echo "================================================================="

build:
	@cmake --preset release
	@cmake --build --preset release -j

debug:
	@cmake --preset debug
	@cmake --build --preset debug -j

test: build
	@ctest --preset release --output-on-failure

preflight:
	@$(PYTHON) scripts/preflight_check.py

demo: build
	@bash demo.sh

format-check:
	@find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format --dry-run --Werror

clean:
	@rm -rf build/ output/ test_tmp.cfg
