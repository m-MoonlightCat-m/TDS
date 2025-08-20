
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDSStaminaComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChange, float, Stamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTDSStaminaComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	FOnStaminaChange OnStaminaChange;

	FTimerHandle TimerHandle_CoolDawnStaminaTimer;
	FTimerHandle TimerHandle_StaminaRecoveryRateTimer;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float Stamina = 100.0f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoveryValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CoolDawnStaminaRecoverTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoveryRate = 0.1f;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetCurrentStamina();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetCurrentStamina(float NewStamina);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void UpdateStaminaValue(float StaminaValue);

	void RecoveryStamina();

	void CoolDawnStaminaEnd();
};
