using FluentAssertions;
using Reqnroll;
using SmartCardStatus.Specs.Services;

namespace SmartCardStatus.Specs.Steps;

[Binding]
public class SmartCardStatusStepDefinitions(ISmartCardStatusService service)
{
  [Given("the smart card status is not inserted")]
  public void GivenTheSmartCardStatusIsNotInserted()
  {
    var status = service.MainWindow.CheckStatus();
    status.Should().Be("No card inserted");
  }

  [Then("the smart card status is inserted")]
  public void ThenTheSmartCardStatusIsInserted()
  {
    var status = service.MainWindow.CheckStatus();
    status.Should().Be("Card inserted");
  }
}
