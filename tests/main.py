import os
import subprocess
import time

# Path to the executable
EXECUTABLE = r"C:\Users\rares\OneDrive\Desktop\roscript\ROScript\interpreter\bin\ros.exe"  # Or "ros.exe" on Windows

# Top-level folder containing all subfolders with .ros files
TEST_ROOT_FOLDER = r"C:\Users\rares\OneDrive\Desktop\roscript\ROScript\tests"

def find_ros_files(folder):
    ros_files = []
    for root, _, files in os.walk(folder):
        for f in files:
            if f.endswith(".ros"):
                ros_files.append(os.path.join(root, f))
    return ros_files

def run_test(file_path):
    with open(file_path, "r") as f:
        source_code = f.read()

    start = time.perf_counter()
    try:
        result = subprocess.run([EXECUTABLE,file_path],
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                timeout=5000)
        duration = time.perf_counter() - start

        passed = result.returncode == 0
        output = result.stdout.decode().strip()
        error = result.stderr.decode().strip()

        return passed, duration, output, error
    except subprocess.TimeoutExpired:
        return False, 5000.0, "", "Timeout"

def main():
    ros_files = find_ros_files(TEST_ROOT_FOLDER)
    total = len(ros_files)
    passed_count = 0

    print(f"Running {total} ROScript test(s)...\n")

    for file_path in ros_files:
        rel_path = os.path.relpath(file_path, TEST_ROOT_FOLDER)
        passed, duration, output, error = run_test(file_path)

        status = "✔ PASSED" if passed else "✖ FAILED"
        print(f"{status} - {rel_path} [{duration:.4f}s]")
        #print(output if output else "  No output")

        if not passed:
            if error:
                print(f"  Error: {error}")
            if output:
                print(f"  Output:\n{output}")
        else:
            passed_count += 1

    print(f"\nSummary: {passed_count}/{total} test(s) passed.")

if __name__ == "__main__":
    main()
