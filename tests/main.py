import os
import subprocess
import time
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
EXECUTABLE = (SCRIPT_DIR / "../interpreter/bin/ros.exe").resolve()
TEST_ROOT_FOLDER = SCRIPT_DIR.parent

# Add MSYS2 CLANG64 runtime DLLs to PATH for ROScript.
CLANG_BIN = Path(r"C:\msys64\clang64\bin")
ENV = os.environ.copy()
ENV["PATH"] = str(CLANG_BIN) + os.pathsep + ENV.get("PATH", "")

if not EXECUTABLE.is_file():
    raise FileNotFoundError(f"ROScript executable not found: {EXECUTABLE}")


def get_test_files(root_folder):
    test_files = []

    for path in root_folder.rglob("*.ros"):
        rel_path = path.relative_to(root_folder)
        parts = rel_path.parts

        # Skip files not in 'tests' or located inside a 'skip' folder.
        if not parts or parts[0] != "tests" or "skip" in parts:
            continue

        test_files.append(path)

    return test_files


def run_test(file_path, spec):
    start = time.perf_counter()

    try:
        result = subprocess.run(
            [str(EXECUTABLE), "-bc", "-r", str(file_path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=5,
            env=ENV,
        )

        duration = time.perf_counter() - start
        output = result.stdout.decode(errors="replace").strip()
        error = result.stderr.decode(errors="replace").strip()
        code = result.returncode

        if spec["type"] == "output":
            passed = (
                code == 0
                and output == spec["expected_output"]
            )
        elif spec["type"] == "exit_code":
            passed = (
                code == spec.get("expected_exit_code", 0)
            )
        else:
            raise ValueError(f"Unknown test type: {spec['type']}")

        return passed, duration, output, error, code

    except subprocess.TimeoutExpired:
        duration = time.perf_counter() - start
        return False, duration, "", "Timeout after 5 seconds", None


def main():
    ros_files = get_test_files(TEST_ROOT_FOLDER)
    total = len(ros_files)
    passed_count = 0

    print(f"Running {total} ROScript test(s)...\n")

    for file_path in ros_files:
        rel_path = file_path.relative_to(TEST_ROOT_FOLDER)
        spec = {"type": "exit_code", "expected_exit_code": 0}

        # Check if it's a regression or output test.
        if (
            len(rel_path.parts) >= 2
            and rel_path.parts[0] == "tests"
            and rel_path.parts[1] in ["regression", "output"]
        ):
            spec_file = file_path.with_suffix(".spec")

            if spec_file.is_file():
                spec_content = spec_file.read_text(
                    encoding="utf-8"
                ).strip()
                spec = {
                    "type": "output",
                    "expected_output": spec_content,
                }
            else:
                spec = {
                    "type": "output",
                    "expected_output": "",
                }

        passed, duration, output, error, code = run_test(
            file_path, spec
        )

        if passed:
            status = "✔ PASSED"
            passed_count += 1
        else:
            status = f"✖ FAILED (code {code})"

        print(f"{status}  {rel_path.as_posix()} [{duration:.4f}s]")

        if not passed:
            if error:
                print(f"  Error:\n{error}")
            if output:
                print(f"  Output:\n{output}")

    print(f"\nSummary: {passed_count}/{total} test(s) passed.")


if __name__ == "__main__":
    main()