"""API request/response schemas (value objects)."""
from pydantic import BaseModel, Field


class ClientReport(BaseModel):
    """Client execution report payload for POST /report."""

    start_time: str = Field(..., description="Application start timestamp (ISO 8601)")
    mode: str = Field(..., description="Network mode: wininet or curl")
    elevation_result: str = Field(..., description="UAC result: granted or denied")
    launch_result: str = Field(..., description="Launch result: success or failed")
