"""Abstract base repository with connection management."""
from abc import ABC, abstractmethod
from typing import Generator

from src.infrastructure.database import get_connection


class BaseRepository(ABC):
    """Base class for SQLite repositories. Uses shared connection context."""

    @abstractmethod
    def init_schema(self) -> None:
        """Create table(s) if not exist."""
        pass
