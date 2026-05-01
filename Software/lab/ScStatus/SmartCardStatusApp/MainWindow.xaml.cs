using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;

namespace SmartCardStatusApp
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window, INotifyPropertyChanged
  {
    public event PropertyChangedEventHandler? PropertyChanged;

    private bool _cardInserted = false;


    public MainWindow()
    {
      InitializeComponent();
      DataContext = this;
    }

    public bool IsInserted
    {
      get => _cardInserted;
      set
      {
        _cardInserted = value;
        OnPropertyChanged();
      }
    }
    protected override void OnActivated(EventArgs e)
    {
      base.OnActivated(e);

      OutputLabel.Content = IsInsertedText;
    }

    private void Button_Click(object sender, RoutedEventArgs e)
    {
      OutputLabel.Content = IsInsertedText;

      //var contextFactory = ContextFactory.Instance;
      //using (var ctx = contextFactory.Establish(SCardScope.System))
      //{
      //    Console.WriteLine("Currently connected readers: ");
      //    var readerNames = ctx.GetReaders();
      //    foreach (var readerName in readerNames)
      //    {
      //        if (readerName == "HID Global OMNIKEY 3x21 Smart Card Reader 0")
      //        {
      //            var status = ctx.GetReaderStatus(readerName);
      //            Console.WriteLine("\t" + readerName + status.CurrentState);

      //            if ((status.EventState & SCRState.Empty) == SCRState.Empty) return;
      //            if ((status.EventState & SCRState.Mute) == SCRState.Mute) return;

      //            using (var reader = ctx.ConnectReader(readerName, SCardShareMode.Shared, SCardProtocol.Any))
      //            {
      //                var cardAtr = reader.GetAttrib(SCardAttribute.AtrString);
      //                Console.WriteLine("ATR: {0}", BitConverter.ToString(cardAtr));
      //                OutputLabel.Content = BitConverter.ToString(cardAtr);
      //                //out = cardAtr;
      //                //Console.ReadKey();
      //            }

      //            using var isoReader = new IsoReader(
      //                context: ctx,
      //                readerName: readerName,
      //                mode: SCardShareMode.Shared,
      //                protocol: SCardProtocol.Any,
      //                releaseContextOnDispose: false);

      //            // Example: SELECT a file
      //            var selectCmd = new CommandApdu(IsoCase.Case4Short, isoReader.ActiveProtocol)
      //            {
      //                CLA = 0x00,
      //                INS = 0xA4,
      //                P1 = 0x04,
      //                P2 = 0x00,
      //                Data = new byte[] { /* AID bytes */ }
      //            };

      //            var response = isoReader.Transmit(selectCmd);
      //            Console.WriteLine(BitConverter.ToString(response.GetData()));
      //            OutputLabel.Content = BitConverter.ToString(response.GetData());

      //            /*
      //             * Smart cards store data in EF (Elementary Files) inside DF (Dedicated Files).
      //             * You must know the file ID, e.g.:
      //             * EF.DG1 for ICAO passports
      //             * EF.PersonalData for some national eIDs
      //             * EF.Name for custom cards*/

      //            // SELECT FILE 0101
      //            var selectFile = new CommandApdu(IsoCase.Case4Short, isoReader.ActiveProtocol)
      //            {
      //                CLA = 0x00,
      //                INS = 0xA4,
      //                P1 = 0x02,
      //                P2 = 0x0C,
      //                Data = new byte[] { 0x01, 0x01 }
      //            };

      //            var selectResp = isoReader.Transmit(selectFile);

      //            // READ BINARY (first 100 bytes)
      //            var readBinary = new CommandApdu(IsoCase.Case2Short, isoReader.ActiveProtocol)
      //            {
      //                CLA = 0x00,
      //                INS = 0xB0,
      //                P1 = 0x00,
      //                P2 = 0x00,
      //                Le = 100
      //            };

      //            var readResp = isoReader.Transmit(readBinary);
      //            var nameBytes = readResp.GetData();
      //            var name = System.Text.Encoding.UTF8.GetString(nameBytes);

      //            Console.WriteLine("Name: " + name);


      //        }
      //    }
      //}
    }

    private string IsInsertedText
    {
      get
      {
        IsInserted = SmartCardStatus.SmartCardStatus.IsCardInserted();
        return IsInserted ? "Card inserted" : "No card inserted";
      }
    }

    protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
  }
}
