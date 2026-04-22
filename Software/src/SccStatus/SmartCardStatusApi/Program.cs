using Microsoft.AspNetCore.Http.HttpResults;
using System.Text.Json.Serialization;

var builder = WebApplication.CreateSlimBuilder(args);

builder.Services.ConfigureHttpJsonOptions(options =>
{
    options.SerializerOptions.TypeInfoResolverChain.Insert(0, AppJsonSerializerContext.Default);
});


var app = builder.Build();

var statusApi = app.MapGroup("/status");
statusApi.MapGet("/", Results<Ok<SmartCardStatusResult>, NotFound> () => {

    try
    {
        var hasInsertedCard = SmartCardStatus.SmartCardStatus.IsCardInserted();
        return TypedResults.Ok(new SmartCardStatusResult(hasInsertedCard));
    }
    catch
    {
        return TypedResults.NotFound();
    }
});

app.Run();

public record SmartCardStatusResult(bool cardInserted);

[JsonSerializable(typeof(SmartCardStatusResult[]))]
internal partial class AppJsonSerializerContext : JsonSerializerContext
{
}
