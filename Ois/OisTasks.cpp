
#include "OisPch.h"
#include "OisTasks.h"
#include "Ois/OisSystem.h"

using namespace Helium;

void ProcessInput()
{
    Input::Capture();
}

void Helium::OisTaskCapture::DefineContract( TaskContract &rContract )
{
    rContract.Fulfills<Helium::StandardDependencies::ReceiveInput>();
}

HELIUM_DEFINE_TASK(OisTaskCapture, ProcessInput)
