"""Database connection factory and context manager."""
import sqlite3
from contextlib import contextmanager
from pathlib import Path
from typing import Generator

from src.config import get_settings


@contextmanager
def get_connection() -> Generator[sqlite3.Connection, None, None]:
    """Yield a database connection. Commits on success, rolls back on exception."""
    db_path = get_settings().project_root / get_settings().db_path
    conn = sqlite3.connect(str(db_path))
    try:
        yield conn
        conn.commit()
    except Exception:
        conn.rollback()
        raise
    finally:
        conn.close()
