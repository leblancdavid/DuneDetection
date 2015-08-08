using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
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
    /// Interaction logic for ImageViewEditor.xaml
    /// </summary>
    public partial class ImageViewEditor : UserControl, INotifyPropertyChanged
    {
        private ImageView _imageView;

        private int _segmentThickness;
        public int SegmentThickness
        {
            get { return _segmentThickness; }
            set 
            { 
                SetField(ref _segmentThickness, value, "SegmentThickness");
                if (_imageView != null)
                {
                    _imageView.UpdateSegmentStyle(_segmentThickness, _selectedColor);
                }
            }

        }

        private Color _selectedColor;
        public Color SelectedColor
        {
            get { return _selectedColor; }
            set 
            { 
                SetField(ref _selectedColor, value, "SelectedColor"); 
                if(_imageView != null)
                {
                    _imageView.UpdateSegmentStyle(_segmentThickness, _selectedColor);
                }
            }
        }

        public ImageViewEditor()
        {
            InitializeComponent();

            SegmentThickness = 3;

            this.DataContext = this;
        }

        public void SetImageView(ImageView imageView)
        {
            _imageView = imageView;
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

        private void ColorBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var color = (e.AddedItems[0]).ToString().Split(' ').Last();
            try
            {
                TypeConverter tc = TypeDescriptor.GetConverter(typeof(Color));
                SelectedColor = (Color)tc.ConvertFromString(color);
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Unable to select color '" + color + "'.");
            }
            
        }
    }
}
