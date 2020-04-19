using CSCore.CoreAudioAPI;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using VolumeRaiser.Interop;

namespace VolumeRaiser
{
    public static class Program
    {
        public static void Main()
        {
            float systemVolume;

            using (MMDevice speakers = AudioUtilities.GetSpeakersDevice())
            using (AudioEndpointVolume volume = AudioEndpointVolume.FromDevice(speakers))
            {
                systemVolume = volume.MasterVolumeLevelScalar;
                Console.WriteLine("Setting volume to " + (volume.MasterVolumeLevelScalar * 100));
            }

            foreach (string app in AudioController.EnumerateApplications())
            {
                Console.WriteLine(app);
            }

            foreach (Process app in GetWindowedProcesses())
            {
                Console.WriteLine("Setting volume for: " + app.ProcessName);
                AudioController.SetApplicationVolume(app.Id, systemVolume);
            }

            Console.ReadLine();
        }

        private static IEnumerable<Process> GetWindowedProcesses()
        {
            foreach (Process element in Process.GetProcesses())
            {
                if (!string.IsNullOrEmpty(element.MainWindowTitle))
                    yield return element;
            }
        }
    }
}
