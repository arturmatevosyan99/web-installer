"""
FilesPServ - Test server for Web Installer.

Layered architecture:
- api: Routes and dependencies
- application: Services (business logic)
- domain: Schemas
- infrastructure: Repositories, database
"""
import logging
import sys

from fastapi import FastAPI

from src.config import get_settings
from src.infrastructure.repositories import QueryRepository, ReportRepository
from src.api import router

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
    handlers=[logging.StreamHandler(sys.stdout)],
)
logger = logging.getLogger(__name__)

app = FastAPI(
    title=get_settings().app_name,
    description="Test server for Web Installer client",
    version="1.0.0",
)

app.include_router(router)


@app.on_event("startup")
async def startup() -> None:
    """Initialize database schema."""
    QueryRepository().init_schema()
    ReportRepository().init_schema()
    logger.info("Database initialized")


def run() -> None:
    """Run the server."""
    import uvicorn
    settings = get_settings()
    uvicorn.run(app, host=settings.host, port=settings.port, reload=False)


if __name__ == "__main__":
    run()
