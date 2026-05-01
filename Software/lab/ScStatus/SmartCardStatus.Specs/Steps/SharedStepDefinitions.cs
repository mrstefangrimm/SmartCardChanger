using Reqnroll;

namespace SmartCardStatus.Specs.Steps;

[Binding]
internal sealed class SharedStepDefinitions
{
  [When("wait for {int} seconds")]
  public void WhenWaitForSeconds(int p0)
  {
    Thread.Sleep(TimeSpan.FromSeconds(p0));
  }
}
