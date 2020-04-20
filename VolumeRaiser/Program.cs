using CSCore.CoreAudioAPI;
using System;

namespace VolumeRaiser
{
    public static class Program
    {
        public static void Main()
        {
            using (MMDeviceEnumerator enumerator = new MMDeviceEnumerator())
            using (MMDevice device = enumerator.GetDefaultAudioEndpoint(DataFlow.Render, Role.Console))
            using (AudioEndpointVolume endpointVolume = AudioEndpointVolume.FromDevice(device))
            using (AudioSessionManager2 sessionManager2 = AudioSessionManager2.FromMMDevice(device))
            using (AudioSessionEnumerator sessionEnumerator = sessionManager2.GetSessionEnumerator())
            {
                Console.WriteLine("Default Render Device: " + device.FriendlyName);
                Console.WriteLine("Master Volume Scalar: " + endpointVolume.GetMasterVolumeLevelScalar());
                Console.WriteLine("\nGetting audio sessions...");

                foreach (AudioSessionControl sessionControl in sessionEnumerator)
                {
                    //Console.WriteLine("Name: " + sessionControl.DisplayName);
                    PrintSessionName(sessionControl);
                    Console.WriteLine("\t- State: " + sessionControl.SessionState.ToString());

                    using (SimpleAudioVolume sessionSimpleVolume = sessionControl.QueryInterface<SimpleAudioVolume>())
                    {
                        Console.WriteLine("\t- Volume: " + sessionSimpleVolume.MasterVolume);
                        sessionSimpleVolume.MasterVolume = 1.0f;
                    }

                    sessionControl.Dispose();
                }
            }
            Console.WriteLine("\nVolumes reset!");
            Console.ReadLine();
        }

        private static void PrintSessionName(AudioSessionControl sessionControl)
        {
            if (!string.IsNullOrEmpty(sessionControl.DisplayName))
            {
                Console.WriteLine("Name: " + sessionControl.DisplayName);
            }
            else
            {
                AudioSessionControl2 sessionControl2 = sessionControl.QueryInterface<AudioSessionControl2>();
                Console.WriteLine("Name: " + sessionControl2.Process.ProcessName);
            }
        }
    }
}
