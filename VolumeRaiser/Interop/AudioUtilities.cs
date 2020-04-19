using CSCore.CoreAudioAPI;
using System.Collections.Generic;

namespace VolumeRaiser.Interop
{
    public static class AudioUtilities
    {
        /// <summary>
        /// Gets all the audio devices attached to the system
        /// </summary>
        /// <returns></returns>
        public static IList<MMDevice> GetAllDevices()
        {
            List<MMDevice> list = new List<MMDevice>();
            using (MMDeviceEnumerator enumerator = new MMDeviceEnumerator())
            using (MMDeviceCollection collection = enumerator.EnumAudioEndpoints(DataFlow.All, DeviceState.All))
            {
                foreach (MMDevice device in collection)
                    list.Add(device);
            }
            return list;
        }

        /// <summary>
        /// Gets the primary audio output device (render + multimedia)
        /// </summary>
        /// <returns></returns>
        public static MMDevice GetSpeakersDevice()
        {
            MMDeviceEnumerator deviceEnumerator = new MMDeviceEnumerator();
            return deviceEnumerator.GetDefaultAudioEndpoint(DataFlow.Render, Role.Multimedia);
        }

        /// <summary>
        /// Gets all the open audio sessions on the system
        /// </summary>
        /// <returns></returns>
        public static IList<AudioSessionControl2> GetAllSessions()
        {
            List<AudioSessionControl2> list = new List<AudioSessionControl2>();
            foreach (AudioSessionControl sCtl in GetAllAudioSessionControls())
                list.Add(sCtl.QueryInterface<AudioSessionControl2>());
            return list;
        }

        internal static IEnumerable<AudioSessionControl> GetAllAudioSessionControls()
        {
            using (AudioSessionManager2 mgr = GetDefaultAudioSessionManager2())
                return mgr.GetSessionEnumerator();
        }

        private static AudioSessionManager2 GetDefaultAudioSessionManager2()
        {
            using (MMDeviceEnumerator enumerator = new MMDeviceEnumerator())
            using (MMDevice speakers = enumerator.GetDefaultAudioEndpoint(DataFlow.Render, Role.Multimedia))
            {
                return AudioSessionManager2.FromMMDevice(speakers);
            }
        }
    }
}
