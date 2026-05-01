using FluentAssertions;
using Reqnroll;
using SmartCardStatus.Specs.Services;

namespace SmartCardStatus.Specs.Steps;

[Binding]
public class SmartCardChangerStepDefinitions(SmartCardChangerService service)
{
  [Given("the smart card changer is in retracted position")]
  public async Task GivenTheSmartCardChangerIsInRetractedPosition()
  {
    var output = await service.App.SendCommand("COM3", "t");
    output.Should().Contain("TrajectoryRetracted");
  }

  [When("the smart card changer slot b is connected")]
  public async Task WhenTheSmartCardChangerSlotBIsConnected()
  {
    await service.App.SendCommand("COM3", "b");
  }

  [Then("the smart chard changer is in connected position")]
  public async Task ThenTheSmartChardChangerIsInConnectedPosition()
  {
    var output = await service.App.SendCommand("COM3", "t");
    output.Should().Contain("TrajectoryConnected");
  }

  [Then("the smart chard changer is retracted")]
  public async Task ThenTheSmartChardChangerIsRetracted()
  {
    await service.App.SendCommand("COM3", "r");
  }
}
