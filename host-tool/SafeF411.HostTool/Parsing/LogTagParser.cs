using System;

namespace SafeF411.HostTool.Parsing;

internal static class LogTagParser
{
    /// <summary>
    /// UART 원문을 그대로 유지한 채 PASS/FAIL 같은 핵심 태그만 최소 판정한다.
    /// 과한 정규식 파싱보다 MVP 단계의 단순 태그 감지를 우선한다.
    /// </summary>
    public static bool ContainsPass(string text)
    {
        return text.Contains("PASS", StringComparison.OrdinalIgnoreCase);
    }

    public static bool ContainsFail(string text)
    {
        return text.Contains("FAIL", StringComparison.OrdinalIgnoreCase);
    }
}
