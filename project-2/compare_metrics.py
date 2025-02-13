import sys
import re

def extract_metrics(file_path):
    metrics = {}
    try:
        with open(file_path, 'r') as file:
            for line in file:
                if "Average Waiting Time" in line:
                    match = re.findall(r"[-+]?(\d*\.\d+|\d+|nan|-nan)", line)
                    metrics["Waiting Time"] = float(match[0]) if match and match[0] not in ["nan", "-nan"] else "nan"
                elif "Average Turnaround Time" in line:
                    match = re.findall(r"[-+]?(\d*\.\d+|\d+|nan|-nan)", line)
                    metrics["Turnaround Time"] = float(match[0]) if match and match[0] not in ["nan", "-nan"] else "nan"
                elif "Average Response Time" in line:
                    match = re.findall(r"[-+]?(\d*\.\d+|\d+|nan|-nan)", line)
                    metrics["Response Time"] = float(match[0]) if match and match[0] not in ["nan", "-nan"] else "nan"
                elif "Total No. of Context Switching Performed" in line:
                    match = re.findall(r"\d+", line)
                    metrics["Context Switches"] = int(match[0]) if match else "nan"
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        sys.exit(1)
    return metrics

def compare_metrics(student_file, sample_file):
    student_metrics = extract_metrics(student_file)
    sample_metrics = extract_metrics(sample_file)

    # Print header
    print("+" + "-" * 21 + "+" + "-" * 21 + "+" + "-" * 21 + "+" + "-" * 12 + "+")
    print("| Performance Metrics | Your Implementation | TA's Implementation | Difference |")
    print("+" + "-" * 21 + "+" + "-" * 21 + "+" + "-" * 21 + "+" + "-" * 12 + "+")

    # Print rows
    for key in sample_metrics.keys():
        student_value = student_metrics.get(key, "N/A")
        sample_value = sample_metrics.get(key, "N/A")
        difference = (
            "nan" if "nan" in (student_value, sample_value) else round(student_value - sample_value, 2)
        )
        print(f"| {key:<19} | {str(student_value):<19} | {str(sample_value):<19} | {str(difference):<10} |")

    # Print footer
    print("+" + "-" * 21 + "+" + "-" * 21 + "+" + "-" * 21 + "+" + "-" * 12 + "+")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 compare_metrics.py <student_output_file> <sample_output_file>")
        sys.exit(1)

    student_file = sys.argv[1]
    sample_file = sys.argv[2]

    compare_metrics(student_file, sample_file)