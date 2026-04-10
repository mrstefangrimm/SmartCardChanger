using PCSC;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SmartCardStatusApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var contextFactory = ContextFactory.Instance;
            using (var ctx = contextFactory.Establish(SCardScope.System))
            {
                Console.WriteLine("Currently connected readers: ");
                var readerNames = ctx.GetReaders();
                foreach (var readerName in readerNames)
                {
                    if (readerName == "HID Global OMNIKEY 3x21 Smart Card Reader 0")
                    {
                        var status = ctx.GetReaderStatus(readerName);
                        Console.WriteLine("\t" + readerName + status.CurrentState);

                        if ((status.EventState & SCRState.Empty) == SCRState.Empty) return;
                        if ((status.EventState & SCRState.Mute) == SCRState.Mute) return;

                        using (var reader = ctx.ConnectReader(readerName, SCardShareMode.Shared, SCardProtocol.Any))
                        {
                            var cardAtr = reader.GetAttrib(SCardAttribute.AtrString);
                            Console.WriteLine("ATR: {0}", BitConverter.ToString(cardAtr));
                            Console.ReadKey();
                        }
                    }
                }
            }
        }
    }
}