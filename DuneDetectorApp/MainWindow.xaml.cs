using Microsoft.Win32;
using System;
using System.Collections.Generic;
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

namespace DuneDetectorApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string _currentImageFileName;
        private BitmapImage _currentImage;


        public MainWindow()
        {
            _currentImageFileName = string.Empty;
            InitializeComponent();
        }

        private void LoadImage_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog loadImageDialog = new OpenFileDialog();

            loadImageDialog.InitialDirectory = "c:\\";
            loadImageDialog.Filter = "jpg files (*.jpg)|*.jpg| png files (*.png)|*.png | bmp files (*.bmp)|*.bmp";
            loadImageDialog.FilterIndex = 1;
            loadImageDialog.RestoreDirectory = true;

            if (loadImageDialog.ShowDialog() == true)
            {
                try
                {
                    _currentImageFileName = loadImageDialog.FileName;
                    _currentImage = new BitmapImage();
                    _currentImage.BeginInit();
                    _currentImage.UriSource = new Uri(_currentImageFileName);
                    _currentImage.EndInit();

                    ImageDisplayView.Source = _currentImage;

                    
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not read image file from disk. Original error: " + ex.Message);
                }
            }
        }

        private void ImagePlotCanvas_MouseMove(object sender, MouseEventArgs e)
        {
            //DuneSegmentsPolyline.Points.Add(e.GetPosition(ImagePlotCanvas));
        }

        private void ProcessButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                using (var duneDetector = new DuneDetector() )
                {
                    var segments = duneDetector.GetDuneSegments(_currentImageFileName);

                    double scaleFactor = (double)ImageDisplayView.ActualWidth / (double)_currentImage.PixelWidth;

                    DuneSegmentsPolylineGrid.Children.Clear();
                    foreach(var seg in segments)
                    {
                        var polyline = new Polyline();
                        polyline.Stroke = new SolidColorBrush(Colors.DarkGreen);
                        polyline.StrokeThickness = 3;
                        foreach(var point in seg.Points)
                        {
                            polyline.Points.Add(new Point { X = point.x * scaleFactor, Y = point.y * scaleFactor });
                        }

                        DuneSegmentsPolylineGrid.Children.Add(polyline);
                    }
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Error while attempting to process image '" + _currentImageFileName + "'. Error Message: " + ex.Message);
            }
        }



        private void ExitProgram_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Application.Current.Shutdown();
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Unable to shutdown program properly: " + ex.Message);
            }
        }

        
    }
}
