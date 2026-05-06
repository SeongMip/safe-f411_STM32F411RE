#nullable enable

namespace SafeF411.HostTool;

partial class MainForm
{
    private System.ComponentModel.IContainer? components = null;
    private System.Windows.Forms.ComboBox cmbPorts = null!;
    private System.Windows.Forms.Button btnRefreshPorts = null!;
    private System.Windows.Forms.Button btnConnect = null!;
    private System.Windows.Forms.Button btnDisconnect = null!;
    private System.Windows.Forms.Label lblConnectionCaption = null!;
    private System.Windows.Forms.Label lblConnectionStatus = null!;
    private System.Windows.Forms.Label lblTestCaption = null!;
    private System.Windows.Forms.Label lblTestStatus = null!;
    private System.Windows.Forms.RichTextBox rtbLog = null!;

    /// <summary>
    /// 디자이너 리소스를 정리한다.
    /// 연결 해제 누락으로 포트가 잠기는 상황을 줄이기 위해 form dispose와 함께 정리한다.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            components?.Dispose();
        }

        base.Dispose(disposing);
    }

    private void InitializeComponent()
    {
        cmbPorts = new System.Windows.Forms.ComboBox();
        btnRefreshPorts = new System.Windows.Forms.Button();
        btnConnect = new System.Windows.Forms.Button();
        btnDisconnect = new System.Windows.Forms.Button();
        lblConnectionCaption = new System.Windows.Forms.Label();
        lblConnectionStatus = new System.Windows.Forms.Label();
        lblTestCaption = new System.Windows.Forms.Label();
        lblTestStatus = new System.Windows.Forms.Label();
        rtbLog = new System.Windows.Forms.RichTextBox();
        SuspendLayout();
        //
        // cmbPorts
        //
        cmbPorts.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
        cmbPorts.FormattingEnabled = true;
        cmbPorts.Location = new System.Drawing.Point(24, 24);
        cmbPorts.Name = "cmbPorts";
        cmbPorts.Size = new System.Drawing.Size(140, 23);
        cmbPorts.TabIndex = 0;
        //
        // btnRefreshPorts
        //
        btnRefreshPorts.Location = new System.Drawing.Point(176, 23);
        btnRefreshPorts.Name = "btnRefreshPorts";
        btnRefreshPorts.Size = new System.Drawing.Size(90, 25);
        btnRefreshPorts.TabIndex = 1;
        btnRefreshPorts.Text = "Refresh";
        btnRefreshPorts.UseVisualStyleBackColor = true;
        btnRefreshPorts.Click += btnRefreshPorts_Click;
        //
        // btnConnect
        //
        btnConnect.Location = new System.Drawing.Point(278, 23);
        btnConnect.Name = "btnConnect";
        btnConnect.Size = new System.Drawing.Size(90, 25);
        btnConnect.TabIndex = 2;
        btnConnect.Text = "Connect";
        btnConnect.UseVisualStyleBackColor = true;
        btnConnect.Click += btnConnect_Click;
        //
        // btnDisconnect
        //
        btnDisconnect.Location = new System.Drawing.Point(380, 23);
        btnDisconnect.Name = "btnDisconnect";
        btnDisconnect.Size = new System.Drawing.Size(90, 25);
        btnDisconnect.TabIndex = 3;
        btnDisconnect.Text = "Disconnect";
        btnDisconnect.UseVisualStyleBackColor = true;
        btnDisconnect.Click += btnDisconnect_Click;
        //
        // lblConnectionCaption
        //
        lblConnectionCaption.AutoSize = true;
        lblConnectionCaption.Location = new System.Drawing.Point(24, 67);
        lblConnectionCaption.Name = "lblConnectionCaption";
        lblConnectionCaption.Size = new System.Drawing.Size(74, 15);
        lblConnectionCaption.TabIndex = 4;
        lblConnectionCaption.Text = "Connection:";
        //
        // lblConnectionStatus
        //
        lblConnectionStatus.AutoSize = true;
        lblConnectionStatus.Location = new System.Drawing.Point(107, 67);
        lblConnectionStatus.Name = "lblConnectionStatus";
        lblConnectionStatus.Size = new System.Drawing.Size(77, 15);
        lblConnectionStatus.TabIndex = 5;
        lblConnectionStatus.Text = "Disconnected";
        //
        // lblTestCaption
        //
        lblTestCaption.AutoSize = true;
        lblTestCaption.Location = new System.Drawing.Point(24, 92);
        lblTestCaption.Name = "lblTestCaption";
        lblTestCaption.Size = new System.Drawing.Size(64, 15);
        lblTestCaption.TabIndex = 6;
        lblTestCaption.Text = "Test state:";
        //
        // lblTestStatus
        //
        lblTestStatus.AutoSize = true;
        lblTestStatus.Location = new System.Drawing.Point(107, 92);
        lblTestStatus.Name = "lblTestStatus";
        lblTestStatus.Size = new System.Drawing.Size(28, 15);
        lblTestStatus.TabIndex = 7;
        lblTestStatus.Text = "IDLE";
        //
        // rtbLog
        //
        rtbLog.Location = new System.Drawing.Point(24, 124);
        rtbLog.Name = "rtbLog";
        rtbLog.ReadOnly = true;
        rtbLog.Size = new System.Drawing.Size(600, 292);
        rtbLog.TabIndex = 8;
        rtbLog.Text = "";
        //
        // MainForm
        //
        AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
        AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        ClientSize = new System.Drawing.Size(650, 440);
        Controls.Add(rtbLog);
        Controls.Add(lblTestStatus);
        Controls.Add(lblTestCaption);
        Controls.Add(lblConnectionStatus);
        Controls.Add(lblConnectionCaption);
        Controls.Add(btnDisconnect);
        Controls.Add(btnConnect);
        Controls.Add(btnRefreshPorts);
        Controls.Add(cmbPorts);
        Name = "MainForm";
        Text = "SafeF411 Host Tool";
        ResumeLayout(false);
        PerformLayout();
    }
}
