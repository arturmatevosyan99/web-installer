"""FastAPI dependency injection."""
from functools import lru_cache

from src.application.services.report_service import ReportService
from src.application.services.file_service import FileService
from src.infrastructure.repositories.report_repo import ReportRepository
from src.infrastructure.repositories.query_repo import QueryRepository


@lru_cache
def get_report_service() -> ReportService:
    """Report service singleton."""
    return ReportService(repository=ReportRepository())


@lru_cache
def get_file_service() -> FileService:
    """File service singleton."""
    return FileService(query_repo=QueryRepository())
