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

namespace DuneDetectorApp.Controls.Views
{
    /// <summary>
    /// Interaction logic for MainImageView.xaml
    /// </summary>
    public partial class ImageView : UserControl
    {
        private string _currentImageFileName;
        private int _segmentThickness;
        private Color _segmentColor;
        private BitmapImage _currentImage;

        public ImageView()
        {
            _currentImageFileName = string.Empty;
            _segmentColor = Colors.Black;
            _segmentThickness = 3;
            InitializeComponent();
        }

        public void LoadImage(string imageFile)
        {
            //clear out the data;
            DuneSegmentsPolylineGrid.Children.Clear();

            _currentImageFileName = imageFile;
            _currentImage = new BitmapImage();
            _currentImage.BeginInit();
            _currentImage.UriSource = new Uri(_currentImageFileName);
            _currentImage.EndInit();

            ImageDisplayView.Source = _currentImage;
        }

        public RenderTargetBitmap GetImage()
        {
            Size size = new Size(ImagePlotCanvas.ActualWidth, ImagePlotCanvas.ActualHeight);
            if (size.IsEmpty || size.Width <= 0 || size.Height <= 0 )
                return null;

            RenderTargetBitmap result = new RenderTargetBitmap((int)size.Width, (int)size.Height, 96, 96, PixelFormats.Pbgra32);

            DrawingVisual drawingvisual = new DrawingVisual();
            using (DrawingContext context = drawingvisual.RenderOpen())
            {
                context.DrawRectangle(new VisualBrush(ImagePlotCanvas), null, new Rect(new Point(), size));
                context.Close();
            }

            result.Render(drawingvisual);
            return result;
        }

        public void SetDuneSegments(DuneSegment[] segments)
        {
            double scaleFactor = (double)ImageDisplayView.ActualWidth / (double)_currentImage.PixelWidth;

            DuneSegmentsPolylineGrid.Children.Clear();
            foreach (var seg in segments)
            {
                var polyline = new Polyline();
                polyline.Stroke = new SolidColorBrush(_segmentColor);
                polyline.StrokeThickness = _segmentThickness;
                foreach (var point in seg.Points)
                {
                    polyline.Points.Add(new Point { X = point.x * scaleFactor, Y = point.y * scaleFactor });
                }

                DuneSegmentsPolylineGrid.Children.Add(polyline);
            }
        }

        public void UpdateSegmentStyle(int thickness, Color color)
        {
            _segmentThickness = thickness;
            _segmentColor = color;
            foreach (var seg in DuneSegmentsPolylineGrid.Children)
            {
                if(seg is Polyline)
                {
                    var polyline = (Polyline)seg;
                    polyline.Stroke = new SolidColorBrush(_segmentColor);
                    polyline.StrokeThickness = _segmentThickness;
                }
            }
        }
    }
}
