using System;
using System.IO.Ports;
using System.Linq;
using System.Windows.Forms;
using SafeF411.HostTool.Models;
using SafeF411.HostTool.Parsing;
using SafeF411.HostTool.Services;

namespace SafeF411.HostTool;

public partial class MainForm : Form
{
    private readonly SerialPortService _serialPortService = new();

    public MainForm()
    {
        InitializeComponent();
        InitializeUi();
        RefreshPortList();
        _serialPortService.TextReceived += SerialPortService_TextReceived;
    }

    private void InitializeUi()
    {
        lblConnectionStatus.Text = "Disconnected";
        lblTestStatus.Text = "IDLE";
        btnDisconnect.Enabled = false;
    }

    private void btnRefreshPorts_Click(object? sender, EventArgs e)
    {
        RefreshPortList();
    }

    private void btnConnect_Click(object? sender, EventArgs e)
    {
        if (cmbPorts.SelectedItem is null)
        {
            MessageBox.Show("COM 포트를 선택하세요.", "Connect", MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }

        try
        {
            string portName = cmbPorts.SelectedItem.ToString()!;
            _serialPortService.Open(portName);

            lblConnectionStatus.Text = $"Connected: {portName}";
            btnConnect.Enabled = false;
            btnDisconnect.Enabled = true;
            cmbPorts.Enabled = false;
        }
        catch (Exception ex)
        {
            MessageBox.Show($"포트 연결 실패\n{ex.Message}", "Connect", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }

    private void btnDisconnect_Click(object? sender, EventArgs e)
    {
        _serialPortService.Close();
        lblConnectionStatus.Text = "Disconnected";
        btnConnect.Enabled = true;
        btnDisconnect.Enabled = false;
        cmbPorts.Enabled = true;
    }

    /// <summary>
    /// 현재 PC에서 보이는 COM 목록만 노출해 사용자가 잘못된 포트를 수동 입력하지 않도록 한다.
    /// </summary>
    private void RefreshPortList()
    {
        string[] ports = SerialPort
            .GetPortNames()
            .OrderBy(name => name, StringComparer.OrdinalIgnoreCase)
            .ToArray();

        cmbPorts.Items.Clear();
        cmbPorts.Items.AddRange(ports);

        if (cmbPorts.Items.Count > 0)
        {
            cmbPorts.SelectedIndex = 0;
        }
    }

    private void SerialPortService_TextReceived(object? sender, string text)
    {
        if (InvokeRequired)
        {
            BeginInvoke(new Action(() => AppendLog(text)));
            return;
        }

        AppendLog(text);
    }

    /// <summary>
    /// 로그 원문을 그대로 남기면서 PASS/FAIL 태그만 별도 상태로 반영한다.
    /// 시험 근거가 되는 UART 원문을 손상시키지 않는 것이 우선이다.
    /// </summary>
    private void AppendLog(string text)
    {
        string normalized = text.Replace("\r\n", "\n").Replace("\r", "\n");
        rtbLog.AppendText(normalized);
        rtbLog.SelectionStart = rtbLog.TextLength;
        rtbLog.ScrollToCaret();

        UartLogEntry entry = new()
        {
            RawText = normalized,
            IsPass = LogTagParser.ContainsPass(normalized),
            IsFail = LogTagParser.ContainsFail(normalized)
        };

        if (entry.IsFail)
        {
            lblTestStatus.Text = "FAIL";
            return;
        }

        if (entry.IsPass)
        {
            lblTestStatus.Text = "PASS";
        }
    }

    protected override void OnFormClosed(FormClosedEventArgs e)
    {
        _serialPortService.Dispose();
        base.OnFormClosed(e);
    }
}
