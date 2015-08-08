using DuneDetectorApp.Controls.Views;
using DuneDetectorApp.Events;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace DuneDetectorApp.Controls.Detectors
{
    /// <summary>
    /// Interaction logic for EBMControl.xaml
    /// </summary>
    public partial class EBMControl : UserControl, INotifyPropertyChanged
    {
        private BackgroundWorker backgroundProcessWorker;
        private ImageView _imageView;
        
        public string CurrentImageFile { get; set; }
        
        private DuneSegment[] _duneSegments;
        public DuneSegment[] DuneSegments
        {
            get
            {
                return _duneSegments;
            }
            protected set
            {
                _duneSegments = value;
                NotifyOfSegmentsUpdated(new DuneSegmentsUpdatedEventArgs(_duneSegments));
            }
        }

        private double _R_Parameter;
        public double R_Parameter 
        {
            get { return _R_Parameter; }
            set 
            {
                SetField(ref _R_Parameter, value, "R_Parameter"); 
            }
        }

        private int _K_Parameter;
        public int K_Parameter
        {
            get { return _K_Parameter; }
            set { SetField(ref _K_Parameter, value, "K_Parameter"); }
        }

        private ObservableCollection<int> _K_Values;
        public ObservableCollection<int> K_Values
        {
            get { return _K_Values; }
            set { SetField(ref _K_Values, value, "K_Values"); }
        }

        private int _minSegmentLength;
        public int MininumSegmentLength
        {
            get { return _minSegmentLength; }
            set 
            {
                if(value < 1)
                {
                    MessageBox.Show("The Minimum Segment Length Must be at least 1.");
                    SetField(ref _minSegmentLength, 1, "MininumSegmentLength"); 
                }
                else
                {
                    SetField(ref _minSegmentLength, value, "MininumSegmentLength"); 
                }
            }
        }

        private double _edgeThreshold;
        public double EdgeThreshold
        {
            get { return _edgeThreshold; }
            set 
            {
                if(value < 0 || value > 6.2832)
                {
                    MessageBox.Show("The Edge Threshold value must be between 0 and 6.2823");
                    SetField(ref _edgeThreshold, 1.5708, "EdgeThreshold"); 
                }
                else
                {
                    SetField(ref _edgeThreshold, value, "EdgeThreshold"); 
                }
            }
        }

        private int _histogramBins;
        public int HistogramBins
        {
            get { return _histogramBins; }
            set 
            {
                if (value < 0 || value > 360)
                {
                    MessageBox.Show("The Number of Bins value must be between 0 and 360");
                    SetField(ref _histogramBins, 16, "HistogramBins");
                }
                else
                {
                    SetField(ref _histogramBins, value, "HistogramBins");
                }
            }
        }

        public event EventHandler<DuneSegmentsUpdatedEventArgs> OnSegmentsUpdated;
        


        public EBMControl()
        {
            InitializeComponent();

            R_Parameter = 0.5;
            MininumSegmentLength = 50;
            K_Values = new ObservableCollection<int>();
            K_Values.Add(3);
            K_Values.Add(5);
            K_Values.Add(7);
            K_Values.Add(9);
            K_Values.Add(11);
            K_Values.Add(13);
            K_Values.Add(15);
            K_Values.Add(17);
            K_Values.Add(19);
            K_Values.Add(21);
            K_Values.Add(23);
            K_Values.Add(25);
            K_Values.Add(27);
            K_Values.Add(29);
            K_Values.Add(31);

            K_Parameter = 9;
            EdgeThreshold = 1.5708;
            HistogramBins = 16;
            
            CurrentImageFile = string.Empty;

            backgroundProcessWorker = new BackgroundWorker();
            backgroundProcessWorker.DoWork += ProcessWorker_DoWork;
            backgroundProcessWorker.RunWorkerCompleted += ProcessWorker_RunWorkerCompleted;

            this.DataContext = this;
        }

        public void SetImageView(ImageView imageView)
        {
            _imageView = imageView;
        }

        private void ProcessButton_Click(object sender, RoutedEventArgs e)
        {
            ProcessButton.Content = "Processing...";
            ProcessButton.IsEnabled = false;
            backgroundProcessWorker.RunWorkerAsync();
        }

        protected virtual void NotifyOfSegmentsUpdated(DuneSegmentsUpdatedEventArgs args)
        {
            if (OnSegmentsUpdated != null)
            {
                OnSegmentsUpdated(this, args);
            }
        }

        void ProcessWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                if (string.IsNullOrEmpty(CurrentImageFile))
                {
                    MessageBox.Show("No image file was loaded!");
                }
                else if (!File.Exists(CurrentImageFile))
                {
                    MessageBox.Show("No file named '" + CurrentImageFile + "' exists!");
                }
                else if (_imageView == null)
                {
                    MessageBox.Show("Error: Image View was not setup properly!");
                }
                else
                {
                    using (var duneDetector = new DuneDetector())
                    {

                        duneDetector.SetParameters(_R_Parameter, _minSegmentLength, _K_Parameter, _edgeThreshold, _histogramBins);
                        DuneSegments = duneDetector.GetDuneSegments(CurrentImageFile);
                        

                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Error while attempting to process image '" + CurrentImageFile + "'. Error Message: " + ex.Message);
            }
            
        }

        void ProcessWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            try
            {
                _imageView.SetDuneSegments(DuneSegments);
                ProcessButton.Content = "Process";
                ProcessButton.IsEnabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Error while attempting to complete the image process image '" + CurrentImageFile + "'. Error Message: " + ex.Message);
            }
            
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
        protected bool SetField<T>(ref T field, T value, string propertyName)
        {
            if (EqualityComparer<T>.Default.Equals(field, value)) return false;
            field = value;
            OnPropertyChanged(propertyName);
            return true;
        }
    }
}
