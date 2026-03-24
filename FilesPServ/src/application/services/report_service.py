"""Report service: business logic for client reports."""
import logging
from typing import Optional

from src.domain import ClientReport
from src.infrastructure.repositories.report_repo import ReportRepository

logger = logging.getLogger(__name__)


class ReportService:
    """Handles saving and processing of client reports."""

    def __init__(self, repository: Optional[ReportRepository] = None) -> None:
        self._repo = repository or ReportRepository()

    def save_report(self, report: ClientReport, client_ip: str) -> None:
        """
        Persist client report to database.

        Args:
            report: Client report payload
            client_ip: Client IP address

        Raises:
            Exception: On database failure
        """
        self._repo.save(
            start_time=report.start_time,
            mode=report.mode,
            elevation_result=report.elevation_result,
            launch_result=report.launch_result,
            client_ip=client_ip,
        )
        logger.info(
            "Report saved: mode=%s elevation=%s launch=%s ip=%s",
            report.mode,
            report.elevation_result,
            report.launch_result,
            client_ip,
        )
