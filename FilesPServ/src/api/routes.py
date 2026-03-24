"""API routes."""
import logging
import random

from fastapi import APIRouter, Request, HTTPException, Depends
from fastapi.responses import FileResponse

from src.domain import ClientReport
from src.config import get_settings
from src.infrastructure.repositories.query_repo import QueryRepository
from src.api.dependencies import get_report_service, get_file_service
from src.application.services.report_service import ReportService
from src.application.services.file_service import FileService

logger = logging.getLogger(__name__)
router = APIRouter()


@router.post("/report")
async def post_report(
    request: Request,
    report: ClientReport,
    service: ReportService = Depends(get_report_service),
) -> dict:
    """Accept client execution report."""
    try:
        service.save_report(report, request.client.host)
        return {"status": "ok"}
    except Exception as e:
        logger.exception("Failed to save report: %s", e)
        raise HTTPException(status_code=500, detail=str(e)) from e


@router.get("/file")
async def get_file(
    request: Request,
    service: FileService = Depends(get_file_service),
) -> FileResponse:
    """Serve distributable file. Simulates 404 for testing."""
    try:
        path = service.get_file_path(request.client.host)
        return FileResponse(str(path), filename="file.exe")
    except FileNotFoundError as e:
        raise HTTPException(status_code=404, detail=str(e)) from e


@router.get("/1")
async def endpoint_1(request: Request) -> dict:
    """Legacy test endpoint. Simulates 404 for testing."""
    QueryRepository().log_access("/1", request.client.host)
    if random.randint(1, get_settings().endpoint_error_rate) == 1:
        raise HTTPException(status_code=404, detail="Item not found")
    return {"message": "ok"}


@router.get("/health")
async def health() -> dict:
    """Health check."""
    return {"status": "ok", "service": get_settings().app_name}
