"""File service: distributable file access and error simulation."""
import logging
import random
from pathlib import Path
from typing import Optional

from src.config import get_settings
from src.infrastructure.repositories.query_repo import QueryRepository

logger = logging.getLogger(__name__)


class FileService:
    """Provides file path and simulates errors for testing."""

    def __init__(
        self,
        query_repo: Optional[QueryRepository] = None,
    ) -> None:
        self._query_repo = query_repo or QueryRepository()
        self._settings = get_settings()

    def get_file_path(self, client_ip: str) -> Path:
        """
        Return path to distributable file. Logs access and may raise 404 for testing.

        Args:
            client_ip: Client IP for logging

        Returns:
            Path to file.exe

        Raises:
            FileNotFoundError: When file does not exist
            PermissionError: Simulated 404 (1/N chance)
        """
        self._query_repo.log_access("/file", client_ip)

        if random.randint(1, self._settings.file_error_rate) == 1:
            logger.debug("Simulated 404 for /file from %s", client_ip)
            raise FileNotFoundError("Item not found")

        path = self._settings.full_file_path
        if not path.exists():
            logger.error("File not found: %s", path)
            raise FileNotFoundError("File not configured")

        return path
