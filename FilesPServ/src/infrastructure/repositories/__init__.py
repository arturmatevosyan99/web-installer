"""Data access repositories."""
from .base import BaseRepository
from .query_repo import QueryRepository
from .report_repo import ReportRepository

__all__ = ["BaseRepository", "QueryRepository", "ReportRepository"]
