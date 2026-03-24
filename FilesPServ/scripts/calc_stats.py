#!/usr/bin/env python3
"""Calculate statistics from ClientReports. Run: py -m scripts.calc_stats"""
import sqlite3
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from src.config import get_settings

DB_PATH = get_settings().project_root / get_settings().db_path


def main() -> None:
    if not DB_PATH.exists():
        print(f"Database not found: {DB_PATH}")
        sys.exit(1)

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    try:
        cursor.execute("SELECT COUNT(*) FROM ClientReports")
        total = cursor.fetchone()[0]
    except sqlite3.OperationalError:
        print("ClientReports table does not exist.")
        conn.close()
        sys.exit(1)

    if total == 0:
        print("No records. Run the client to collect data.")
        conn.close()
        sys.exit(0)

    cursor.execute("SELECT elevation_result, launch_result FROM ClientReports")
    rows = cursor.fetchall()
    granted = sum(1 for r in rows if r[0] == "granted")
    denied = sum(1 for r in rows if r[0] == "denied")
    launch_ok = sum(1 for r in rows if r[1] == "success")

    print("=" * 50)
    print("Client Reports Statistics")
    print("=" * 50)
    print(f"Total runs: {total}")
    print(f"Elevation granted: {granted} ({granted/total*100:.1f}%)")
    print(f"Elevation denied:  {denied} ({denied/total*100:.1f}%)")
    print(f"Launch success:    {launch_ok} ({launch_ok/total*100:.1f}%)")
    print("=" * 50)

    out = get_settings().project_root / "stats_results.csv"
    with open(out, "w", encoding="utf-8") as f:
        f.write("metric,count,percent\n")
        f.write(f"total,{total},100\n")
        f.write(f"elevation_granted,{granted},{granted/total*100:.2f}\n")
        f.write(f"elevation_denied,{denied},{denied/total*100:.2f}\n")
        f.write(f"launch_success,{launch_ok},{launch_ok/total*100:.2f}\n")
    print(f"Saved to {out}")
    conn.close()


if __name__ == "__main__":
    main()
