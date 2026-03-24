"""Application settings loaded from environment."""
from pathlib import Path
from functools import lru_cache

try:
    from pydantic_settings import BaseSettings
except ImportError:
    from pydantic import BaseSettings  # type: ignore


class Settings(BaseSettings):
    """Application configuration. Override via FILESPSERV_* env vars."""

    app_name: str = "FilesPServ"
    host: str = "0.0.0.0"
    port: int = 8000
    db_path: str = "Queries.db"
    file_path: str = "file/file.exe"
    file_error_rate: int = 10  # 1 in N requests return 404 for /file
    endpoint_error_rate: int = 100  # 1 in N requests return 404 for /1

    @property
    def project_root(self) -> Path:
        """Project root directory (FilesPServ)."""
        return Path(__file__).resolve().parent.parent.parent

    @property
    def full_file_path(self) -> Path:
        """Absolute path to distributable file."""
        return self.project_root / self.file_path

    model_config = {"env_prefix": "FILESPSERV_", "extra": "ignore"}


@lru_cache
def get_settings() -> Settings:
    """Cached settings singleton."""
    return Settings()
