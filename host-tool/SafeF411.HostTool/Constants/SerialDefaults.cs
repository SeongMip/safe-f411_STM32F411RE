using System.IO.Ports;

namespace SafeF411.HostTool.Constants;

internal static class SerialDefaults
{
    /// <summary>
    /// safe-f411 UART 기본 설정과 host tool의 초기값을 맞춰 연결 실수로 인한 관찰 실패를 줄인다.
    /// </summary>
    public const int BaudRate = 115200;

    public const int DataBits = 8;

    public const Parity Parity = System.IO.Ports.Parity.None;

    public const StopBits StopBits = System.IO.Ports.StopBits.One;
}
