using System;
using System.Windows.Forms;

namespace SafeF411.HostTool;

internal static class Program
{
    /// <summary>
    /// Windows host tool의 진입점이다.
    /// 보드 UART 로그를 단일 화면에서 관찰할 수 있도록 메인 폼을 실행한다.
    /// </summary>
    [STAThread]
    private static void Main()
    {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new MainForm());
    }
}
