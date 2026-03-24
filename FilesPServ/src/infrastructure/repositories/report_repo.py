"""Repository for client reports (ClientReports table)."""
from src.infrastructure.repositories.base import BaseRepository
from src.infrastructure.database import get_connection


class ReportRepository(BaseRepository):
    """Stores client execution reports."""

    def init_schema(self) -> None:
        """Create ClientReports table if not exists."""
        with get_connection() as conn:
            conn.execute("""
                CREATE TABLE IF NOT EXISTS ClientReports (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    start_time TEXT NOT NULL,
                    mode TEXT NOT NULL,
                    elevation_result TEXT NOT NULL,
                    launch_result TEXT NOT NULL,
                    client_ip TEXT NOT NULL,
                    datetime DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            """)

    def save(
        self,
        start_time: str,
        mode: str,
        elevation_result: str,
        launch_result: str,
        client_ip: str,
    ) -> None:
        """Persist a client report."""
        with get_connection() as conn:
            conn.execute("""
                INSERT INTO ClientReports (start_time, mode, elevation_result, launch_result, client_ip)
                VALUES (?, ?, ?, ?, ?)
            """, (start_time, mode, elevation_result, launch_result, client_ip))
