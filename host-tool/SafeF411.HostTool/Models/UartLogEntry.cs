using System;

namespace SafeF411.HostTool.Models;

internal sealed class UartLogEntry
{
    public DateTime Timestamp { get; init; } = DateTime.Now;

    public string RawText { get; init; } = string.Empty;

    public bool IsPass { get; init; }

    public bool IsFail { get; init; }
}
