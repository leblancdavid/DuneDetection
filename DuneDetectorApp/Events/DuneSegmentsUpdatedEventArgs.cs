using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DuneDetectorApp.Events
{
    public class DuneSegmentsUpdatedEventArgs : EventArgs
    {
        public DuneSegment[] Segments { get; private set; }
        public DuneSegmentsUpdatedEventArgs(DuneSegment[] segments)
        {
            Segments = segments;
        }
    }
}
