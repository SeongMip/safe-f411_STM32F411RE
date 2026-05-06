using System;
using System.IO.Ports;
using SafeF411.HostTool.Constants;

namespace SafeF411.HostTool.Services;

internal sealed class SerialPortService : IDisposable
{
    private readonly SerialPort _serialPort;

    public event EventHandler<string>? TextReceived;

    public bool IsOpen => _serialPort.IsOpen;

    public SerialPortService()
    {
        _serialPort = new SerialPort
        {
            BaudRate = SerialDefaults.BaudRate,
            DataBits = SerialDefaults.DataBits,
            Parity = SerialDefaults.Parity,
            StopBits = SerialDefaults.StopBits,
            NewLine = "\r\n"
        };

        _serialPort.DataReceived += OnDataReceived;
    }

    /// <summary>
    /// 선택한 COM 포트를 열어 보드 로그 관찰을 시작한다.
    /// 연결 전에 기존 포트를 닫아 재연결 시 상태 꼬임을 줄인다.
    /// </summary>
    public void Open(string portName)
    {
        if (string.IsNullOrWhiteSpace(portName))
        {
            throw new ArgumentException("포트 이름이 비어 있습니다.", nameof(portName));
        }

        if (_serialPort.IsOpen)
        {
            _serialPort.Close();
        }

        _serialPort.PortName = portName;
        _serialPort.Open();
    }

    public void Close()
    {
        if (_serialPort.IsOpen)
        {
            _serialPort.Close();
        }
    }

    private void OnDataReceived(object? sender, SerialDataReceivedEventArgs e)
    {
        string text = _serialPort.ReadExisting();
        if (!string.IsNullOrEmpty(text))
        {
            TextReceived?.Invoke(this, text);
        }
    }

    public void Dispose()
    {
        _serialPort.DataReceived -= OnDataReceived;
        Close();
        _serialPort.Dispose();
    }
}
