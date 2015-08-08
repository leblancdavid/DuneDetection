using Microsoft.Win32;
using System;
using System.Collections.Generic;
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
using System.Xml;

namespace DuneDetectorApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            EBMDuneDetector.OnSegmentsUpdated += EBMDuneDetector_OnSegmentsUpdated;
            EBMDuneDetector.SetImageView(ProcessImageView);
            ViewEditorSettingsControl.SetImageView(ProcessImageView);
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
                ProcessImageView.LoadImage(loadImageDialog.FileName);
                EBMDuneDetector.CurrentImageFile = loadImageDialog.FileName;
            }
        }

        private void ImagePlotCanvas_MouseMove(object sender, MouseEventArgs e)
        {
            //DuneSegmentsPolyline.Points.Add(e.GetPosition(ImagePlotCanvas));
        }

        void EBMDuneDetector_OnSegmentsUpdated(object sender, Events.DuneSegmentsUpdatedEventArgs e)
        {
            //ProcessImageView.SetDuneSegments(e.Segments);
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

        private void SavePNGFormat_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SaveImageFormat(new PngBitmapEncoder(), ".png");
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Unable to save image as .png: " + ex.Message);
            }
            
        }

        private void SaveJPGFormat_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SaveImageFormat(new JpegBitmapEncoder(), ".jpg");
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Unable to save image as .jpg: " + ex.Message);
            }
            
        }

        private void SaveBMPFormat_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SaveImageFormat(new BmpBitmapEncoder(), ".bmp");
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Unable to save image as .bmp: " + ex.Message);
            }
        }

        private void SaveImageFormat(BitmapEncoder encoder, string extention)
        {
            var imageToSave = ProcessImageView.GetImage();
            if(imageToSave != null)
            {
                SaveFileDialog saveImageDialog = new SaveFileDialog();
                saveImageDialog.InitialDirectory = "c:\\";
                saveImageDialog.Filter = "image file (*" + extention + ")|*" + extention;
                saveImageDialog.FilterIndex = 1;
                saveImageDialog.RestoreDirectory = true;

                if (saveImageDialog.ShowDialog() == true)
                {
                    FileStream file = new FileStream(saveImageDialog.FileName, FileMode.Create);
                    encoder.Frames.Add(BitmapFrame.Create(imageToSave));
                    encoder.Save(file);
                }
            }
            else
            {
                MessageBox.Show("Unable to Save Image: No image has been loaded!");
            }
            
        }

        private void SaveDataXMLFormat_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if(EBMDuneDetector.DuneSegments != null && EBMDuneDetector.DuneSegments.Count() > 0)
                {
                    SaveFileDialog saveXmlData = new SaveFileDialog();
                    saveXmlData.InitialDirectory = "c:\\";
                    saveXmlData.Filter = "xml file (*.xml)|*.xml";
                    saveXmlData.FilterIndex = 1;
                    saveXmlData.RestoreDirectory = true;

                    if (saveXmlData.ShowDialog() == true)
                    {
                        SaveXMLSegmentData(saveXmlData.FileName);
                    }
                }
                else
                {
                    MessageBox.Show("Unable to Save Data: No image or data has been loaded!");
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: Unable to save data in XML format: " + ex.Message);
            }
        }

        private void SaveDataTXTFormat_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (EBMDuneDetector.DuneSegments != null && EBMDuneDetector.DuneSegments.Count() > 0)
                {
                    SaveFileDialog saveTxtData = new SaveFileDialog();
                    saveTxtData.InitialDirectory = "c:\\";
                    saveTxtData.Filter = "txt file (*.txt)|*.txt";
                    saveTxtData.FilterIndex = 1;
                    saveTxtData.RestoreDirectory = true;

                    if (saveTxtData.ShowDialog() == true)
                    {
                        SaveTXTSegmentData(saveTxtData.FileName);
                    }
                }
                else
                {
                    MessageBox.Show("Unable to Save Data: No image or data has been loaded!");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Unable to save data in TXT format: " + ex.Message);
            }
        }

        private void SaveXMLSegmentData(string fileName)
        {
            var duneSeg = EBMDuneDetector.DuneSegments;

            XmlDocument xmlDoc = new XmlDocument();
            var rootNode = xmlDoc.CreateElement("DuneSegments");
            var attr = xmlDoc.CreateAttribute("count");
            attr.Value = duneSeg.Count().ToString();
            rootNode.Attributes.Append(attr);
            xmlDoc.AppendChild(rootNode);

            foreach (var segment in duneSeg)
            {
                var segNode = xmlDoc.CreateElement("segment");
                attr = xmlDoc.CreateAttribute("length");
                attr.Value = segment.Points.Count().ToString();
                segNode.Attributes.Append(attr);
                rootNode.AppendChild(segNode);
                foreach (var point in segment.Points)
                {
                    var pointNode = xmlDoc.CreateElement("point");
                    attr = xmlDoc.CreateAttribute("x");
                    attr.Value = point.x.ToString();
                    pointNode.Attributes.Append(attr);
                    attr = xmlDoc.CreateAttribute("y");
                    attr.Value = point.y.ToString();
                    pointNode.Attributes.Append(attr);
                    segNode.AppendChild(pointNode);
                }
            }

            xmlDoc.Save(fileName);
        }

        private void SaveTXTSegmentData(string fileName)
        {
            var duneSeg = EBMDuneDetector.DuneSegments;

            StringBuilder sb = new StringBuilder();
            foreach (var segment in duneSeg)
            {
                foreach (var point in segment.Points)
                {
                    sb.Append(point.x.ToString() + "," + point.y.ToString() + ";");
                }
                sb.AppendLine();
            }
            using (StreamWriter outfile = new StreamWriter(fileName))
            {
                outfile.Write(sb.ToString());
            }

        }

        private void SaveAll_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (EBMDuneDetector.DuneSegments != null && EBMDuneDetector.DuneSegments.Count() > 0)
                {
                    SaveFileDialog saveAllData = new SaveFileDialog();
                    saveAllData.InitialDirectory = "c:\\";
                    saveAllData.RestoreDirectory = true;

                    if (saveAllData.ShowDialog() == true)
                    {
                        SaveXMLSegmentData(saveAllData.FileName + ".xml");
                        SaveTXTSegmentData(saveAllData.FileName + ".txt");
                        SaveImageFormat(new JpegBitmapEncoder(), ".jpg");
                    }
                }
                else
                {
                    MessageBox.Show("Unable to Save Data: No image or data has been loaded!");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Unable to save data in TXT format: " + ex.Message);
            }
        }

        
    }
}
