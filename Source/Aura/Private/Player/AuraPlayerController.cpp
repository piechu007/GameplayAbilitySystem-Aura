// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"

#include "Interaction/TargetableInterface.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
    FHitResult CurosorHit;
    GetHitResultUnderCursor(ECC_Visibility, false, CurosorHit);
    if (!CurosorHit.bBlockingHit)
        return;

    LastTarget = CurrentTarget;
    CurrentTarget = CurosorHit.GetActor();

    if (LastTarget)
    {
        if (CurrentTarget)
        {
            if (CurrentTarget == LastTarget) // Last targed is valid, and curren targed is valid, and the same target
            {
                // nothing
            }
            else // Last targed is valid, and curren targed is valid, and diferent targets
            {
                CurrentTarget->HighlightActor();
                LastTarget->UnHighlightActor();
            }
        }
        else // Last targed is valid, and curren targed is null
        {
            LastTarget->UnHighlightActor();
        }
    }
    else // Last targed is null
    {
        if (CurrentTarget) // Last targed is null, and curren targed is valid
        {
            CurrentTarget->HighlightActor();
        }
        else // Last targed is null, and curren targed is null
        {
            // nothing
        }
    }
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();
    check(AuraContext);

    UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem)
    {
        Subsystem->AddMappingContext(AuraContext, 0);
    }

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue &InputActionValue)
{
    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn *ControlledPawn = GetPawn<APawn>())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}
