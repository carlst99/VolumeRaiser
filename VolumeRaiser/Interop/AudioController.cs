using CSCore.CoreAudioAPI;
using System.Collections.Generic;

namespace VolumeRaiser.Interop
{
    internal static class AudioController
    {
        /// <summary>
        /// Sets the volume for a process, provided that process holds an audio session
        /// </summary>
        /// <param name="processId"></param>
        /// <param name="level">A volume in the range 0 - 1.0</param>
        public static void SetApplicationVolume(int processId, float level)
        {
            SimpleAudioVolume volume = GetVolumeObject(processId);
            if (volume == null)
                return;
            volume.MasterVolume = level;
        }

        /// <summary>
        /// Sets the mute status for a process, provided that process holds an audio session
        /// </summary>
        /// <param name="processId"></param>
        /// <param name="mute"></param>
        public static void SetApplicationMute(int processId, bool mute)
        {
            SimpleAudioVolume volume = GetVolumeObject(processId);
            if (volume == null)
                return;
            volume.IsMuted = mute;
        }

        /// <summary>
        /// Gets the display names of all current audio sessions
        /// </summary>
        /// <returns></returns>
        public static IEnumerable<string> EnumerateApplications()
        {
            foreach (AudioSessionControl sCtl in AudioUtilities.GetAllAudioSessionControls())
            {
                yield return sCtl.DisplayName;
                sCtl.Dispose();
            }
        }

        /// <summary>
        /// Gets a <see cref="SimpleAudioVolume"/> object for a given process, provided that process holds an audio session
        /// </summary>
        /// <param name="processId"></param>
        /// <returns></returns>
        internal static SimpleAudioVolume GetVolumeObject(int processId)
        {
            foreach (AudioSessionControl sCtl in AudioUtilities.GetAllAudioSessionControls())
            {
                using (var sCtl2 = sCtl.QueryInterface<AudioSessionControl2>())
                using (var simpleVolume = sCtl.QueryInterface<SimpleAudioVolume>())
                {
                    if (sCtl2.ProcessID == processId)
                        return simpleVolume;
                }
                sCtl.Dispose();
            }
            return null;
        }
    }
}
