from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path


def _project_root() -> Path:
    return Path(__file__).resolve().parents[2]


def _run(
    command: list[str], cwd: Path, env: dict[str, str] | None = None
) -> None:
    print(f"$ {' '.join(command)}")
    subprocess.run(command, cwd=cwd, env=env, check=True)


def main() -> None:
    print("Papaya scripts")
    print("- uv run gen-stubs")


def gen_stubs() -> None:
    root = _project_root()
    build_dir = root / "build"

    _run(["cmake", "-S", ".", "-B", str(build_dir)], cwd=root)
    _run(["cmake", "--build", str(build_dir), "-j4"], cwd=root)

    env = os.environ.copy()
    env["PYTHONPATH"] = str(build_dir)
    _run(
        [
            sys.executable,
            "-m",
            "pybind11_stubgen",
            "papaya_backend",
            "-o",
            "typings",
        ],
        cwd=root,
        env=env,
    )

    print("Stubs generados en typings/papaya_backend.pyi")
