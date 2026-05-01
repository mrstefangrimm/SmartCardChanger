using FlaUI.Core.AutomationElements;
using Reqnroll.Amp;

namespace SmartCardStatus.Specs.Apps;

public class SmartCardStatusAppMainWindow(FlaUIDriver driver)
{
  public string CheckStatus()
  {
    CheckButton.Click();

    return OutputLabel.Text;
  }

  private Label OutputLabel => driver.Stub.FindFirstDescendant("OutputLabelAid").AsLabel();
  private Button CheckButton => driver.Stub.FindFirstDescendant("CheckButtonAid").AsButton();
}
