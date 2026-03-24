"""Repository for request logging (Queries table)."""
from src.infrastructure.repositories.base import BaseRepository
from src.infrastructure.database import get_connection


class QueryRepository(BaseRepository):
    """Logs endpoint access with client IP."""

    def init_schema(self) -> None:
        """Create Queries table if not exists."""
        with get_connection() as conn:
            conn.execute("""
                CREATE TABLE IF NOT EXISTS Queries (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    endpoint TEXT NOT NULL,
                    ip TEXT NOT NULL,
                    datetime DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            """)

    def log_access(self, endpoint: str, client_ip: str) -> None:
        """Log endpoint access."""
        with get_connection() as conn:
            conn.execute(
                "INSERT INTO Queries (endpoint, ip) VALUES (?, ?)",
                (endpoint, client_ip),
            )
