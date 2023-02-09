/*

EE Syscalls which automatically detect kmode.

*/

.set noat
.set noreorder

#include "regdef.h"

.global RFU000_FullReset
.ent RFU000_FullReset
RFU000_FullReset:
                        j       __syscall
                        addiu   v1, zero, 0
.end RFU000_FullReset

.global ResetEE
.ent ResetEE
ResetEE:
                        j       __syscall
                        addiu   v1, zero, 1
.end ResetEE

.global SetGsCrt
.ent SetGsCrt
SetGsCrt:
                        j       __syscall
                        addiu   v1, zero, 2
.end SetGsCrt

.global RFU003
.ent RFU003
RFU003:
                        j       __syscall
                        addiu   v1, zero, 3
.end RFU003

.global Exit
.ent Exit
Exit:
                        j       __syscall
                        addiu   v1, zero, 4
.end Exit

.global RFU005
.ent RFU005
RFU005:
                        j       __syscall
                        addiu   v1, zero, 5
.end RFU005

.global LoadPS2Exe
.ent LoadPS2Exe
LoadPS2Exe:
                        j       __syscall
                        addiu   v1, zero, 6
.end LoadPS2Exe

.global ExecPS2
.ent ExecPS2
ExecPS2:
                        j       __syscall
                        addiu   v1, zero, 7
.end ExecPS2

.global RFU008
.ent RFU008
RFU008:
                        j       __syscall
                        addiu   v1, zero, 8
.end RFU008

.global RFU009
.ent RFU009
RFU009:
                        j       __syscall
                        addiu   v1, zero, 9
.end RFU009

.global AddSbusIntcHandler
.ent AddSbusIntcHandler
AddSbusIntcHandler:
                        j       __syscall
                        addiu   v1, zero, 10
.end AddSbusIntcHandler

.global RemoveSbusIntcHandler
.ent RemoveSbusIntcHandler
RemoveSbusIntcHandler:
                        j       __syscall
                        addiu   v1, zero, 11
.end RemoveSbusIntcHandler

.global Interrupt2Iop
.ent Interrupt2Iop
Interrupt2Iop:
                        j       __syscall
                        addiu   v1, zero, 12
.end Interrupt2Iop

.global SetVTLBRefillHandler
.ent SetVTLBRefillHandler
SetVTLBRefillHandler:
                        j       __syscall
                        addiu   v1, zero, 13
.end SetVTLBRefillHandler

.global SetVCommonHandler
.ent SetVCommonHandler
SetVCommonHandler:
                        j       __syscall
                        addiu   v1, zero, 14
.end SetVCommonHandler

.global SetVInterruptHandler
.ent SetVInterruptHandler
SetVInterruptHandler:
                        j       __syscall
                        addiu   v1, zero, 15
.end SetVInterruptHandler

.global AddIntcHandler
.ent AddIntcHandler
AddIntcHandler:
                        j       __syscall
                        addiu   v1, zero, 16
.end AddIntcHandler

.global RemoveIntcHandler
.ent RemoveIntcHandler
RemoveIntcHandler:
                        j       __syscall
                        addiu   v1, zero, 17
.end RemoveIntcHandler

.global AddDmacHandler
.ent AddDmacHandler
AddDmacHandler:
                        j       __syscall
                        addiu   v1, zero, 18
.end AddDmacHandler

.global RemoveDmacHandler
.ent RemoveDmacHandler
RemoveDmacHandler:
                        j       __syscall
                        addiu   v1, zero, 19
.end RemoveDmacHandler

.global _EnableIntc
.ent _EnableIntc
_EnableIntc:
                        j       __syscall
                        addiu   v1, zero, 20
.end _EnableIntc

.global _DisableIntc
.ent _DisableIntc
_DisableIntc:
                        j       __syscall
                        addiu   v1, zero, 21
.end _DisableIntc

.global _EnableDmac
.ent _EnableDmac
_EnableDmac:
                        j       __syscall
                        addiu   v1, zero, 22
.end _EnableDmac

.global _DisableDmac
.ent _DisableDmac
_DisableDmac:
                        j       __syscall
                        addiu   v1, zero, 23
.end _DisableDmac

.global _SetAlarm
.ent _SetAlarm
_SetAlarm:
                        j       __syscall
                        addiu   v1, zero, 24
.end _SetAlarm

.global _ReleaseAlarm
.ent _ReleaseAlarm
_ReleaseAlarm:
                        j       __syscall
                        addiu   v1, zero, 25
.end _ReleaseAlarm

.global _iEnableIntc
.ent _iEnableIntc
_iEnableIntc:
                        j       __syscall
                        addiu   v1, zero, -26
.end _iEnableIntc

.global _iDisableIntc
.ent _iDisableIntc
_iDisableIntc:
                        j       __syscall
                        addiu   v1, zero, -27
.end _iDisableIntc

.global _iEnableDmac
.ent _iEnableDmac
_iEnableDmac:
                        j       __syscall
                        addiu   v1, zero, -28
.end _iEnableDmac

.global _iDisableDmac
.ent _iDisableDmac
_iDisableDmac:
                        j       __syscall
                        addiu   v1, zero, -29
.end _iDisableDmac

.global _iSetAlarm
.ent _iSetAlarm
_iSetAlarm:
                        j       __syscall
                        addiu   v1, zero, -30
.end _iSetAlarm

.global _iReleaseAlarm
.ent _iReleaseAlarm
_iReleaseAlarm:
                        j       __syscall
                        addiu   v1, zero, -31
.end _iReleaseAlarm

.global CreateThread
.ent CreateThread
CreateThread:
                        j       __syscall
                        addiu   v1, zero, 32
.end CreateThread

.global DeleteThread
.ent DeleteThread
DeleteThread:
                        j       __syscall
                        addiu   v1, zero, 33
.end DeleteThread

.global StartThread
.ent StartThread
StartThread:
                        j       __syscall
                        addiu   v1, zero, 34
.end StartThread

.global ExitThread
.ent ExitThread
ExitThread:
                        j       __syscall
                        addiu   v1, zero, 35
.end ExitThread

.global ExitDeleteThread
.ent ExitDeleteThread
ExitDeleteThread:
                        j       __syscall
                        addiu   v1, zero, 36
.end ExitDeleteThread

.global TerminateThread
.ent TerminateThread
TerminateThread:
                        j       __syscall
                        addiu   v1, zero, 37
.end TerminateThread

.global iTerminateThread
.ent iTerminateThread
iTerminateThread:
                        j       __syscall
                        addiu   v1, zero, -38
.end iTerminateThread

.global DisableDispatchThread
.ent DisableDispatchThread
DisableDispatchThread:
                        j       __syscall
                        addiu   v1, zero, 39
.end DisableDispatchThread

.global EnableDispatchThread
.ent EnableDispatchThread
EnableDispatchThread:
                        j       __syscall
                        addiu   v1, zero, 40
.end EnableDispatchThread

.global ChangeThreadPriority
.ent ChangeThreadPriority
ChangeThreadPriority:
                        j       __syscall
                        addiu   v1, zero, 41
.end ChangeThreadPriority

.global iChangeThreadPriority
.ent iChangeThreadPriority
iChangeThreadPriority:
                        j       __syscall
                        addiu   v1, zero, -42
.end iChangeThreadPriority

.global RotateThreadReadyQueue
.ent RotateThreadReadyQueue
RotateThreadReadyQueue:
                        j       __syscall
                        addiu   v1, zero, 43
.end RotateThreadReadyQueue

.global iRotateThreadReadyQueue
.ent iRotateThreadReadyQueue
iRotateThreadReadyQueue:
                        j       __syscall
                        addiu   v1, zero, -44
.end iRotateThreadReadyQueue

.global ReleaseWaitThread
.ent ReleaseWaitThread
ReleaseWaitThread:
                        j       __syscall
                        addiu   v1, zero, 45
.end ReleaseWaitThread

.global iReleaseWaitThread
.ent iReleaseWaitThread
iReleaseWaitThread:
                        j       __syscall
                        addiu   v1, zero, -46
.end iReleaseWaitThread

.global GetThreadId
.ent GetThreadId
GetThreadId:
                        j       __syscall
                        addiu   v1, zero, 47
.end GetThreadId

.global ReferThreadStatus
.ent ReferThreadStatus
ReferThreadStatus:
                        j       __syscall
                        addiu   v1, zero, 48
.end ReferThreadStatus

.global iReferThreadStatus
.ent iReferThreadStatus
iReferThreadStatus:
                        j       __syscall
                        addiu   v1, zero, -49
.end iReferThreadStatus

.global SleepThread
.ent SleepThread
SleepThread:
                        j       __syscall
                        addiu   v1, zero, 50
.end SleepThread

.global WakeupThread
.ent WakeupThread
WakeupThread:
                        j       __syscall
                        addiu   v1, zero, 51
.end WakeupThread

.global iWakeupThread
.ent iWakeupThread
iWakeupThread:
                        j       __syscall
                        addiu   v1, zero, -52
.end iWakeupThread

.global CancelWakeupThread
.ent CancelWakeupThread
CancelWakeupThread:
                        j       __syscall
                        addiu   v1, zero, 53
.end CancelWakeupThread

.global iCancelWakeupThread
.ent iCancelWakeupThread
iCancelWakeupThread:
                        j       __syscall
                        addiu   v1, zero, -54
.end iCancelWakeupThread

.global SuspendThread
.ent SuspendThread
SuspendThread:
                        j       __syscall
                        addiu   v1, zero, 55
.end SuspendThread

.global iSuspendThread
.ent iSuspendThread
iSuspendThread:
                        j       __syscall
                        addiu   v1, zero, -56
.end iSuspendThread

.global ResumeThread
.ent ResumeThread
ResumeThread:
                        j       __syscall
                        addiu   v1, zero, 57
.end ResumeThread

.global iResumeThread
.ent iResumeThread
iResumeThread:
                        j       __syscall
                        addiu   v1, zero, -58
.end iResumeThread

.global JoinThread
.ent JoinThread
JoinThread:
                        j       __syscall
                        addiu   v1, zero, 59
.end JoinThread

.global RFU060
.ent RFU060
RFU060:
                        j       __syscall
                        addiu   v1, zero, 60
.end RFU060

.global RFU061
.ent RFU061
RFU061:
                        j       __syscall
                        addiu   v1, zero, 61
.end RFU061

.global EndOfHeap
.ent EndOfHeap
EndOfHeap:
                        j       __syscall
                        addiu   v1, zero, 62
.end EndOfHeap

.global RFU063
.ent RFU063
RFU063:
                        j       __syscall
                        addiu   v1, zero, 63
.end RFU063

.global CreateSema
.ent CreateSema
CreateSema:
                        j       __syscall
                        addiu   v1, zero, 64
.end CreateSema

.global DeleteSema
.ent DeleteSema
DeleteSema:
                        j       __syscall
                        addiu   v1, zero, 65
.end DeleteSema

.global SignalSema
.ent SignalSema
SignalSema:
                        j       __syscall
                        addiu   v1, zero, 66
.end SignalSema

.global iSignalSema
.ent iSignalSema
iSignalSema:
                        j       __syscall
                        addiu   v1, zero, -67
.end iSignalSema

.global WaitSema
.ent WaitSema
WaitSema:
                        j       __syscall
                        addiu   v1, zero, 68
.end WaitSema

.global PollSema
.ent PollSema
PollSema:
                        j       __syscall
                        addiu   v1, zero, 69
.end PollSema

.global iPollSema
.ent iPollSema
iPollSema:
                        j       __syscall
                        addiu   v1, zero, -70
.end iPollSema

.global ReferSemaStatus
.ent ReferSemaStatus
ReferSemaStatus:
                        j       __syscall
                        addiu   v1, zero, 71
.end ReferSemaStatus

.global iReferSemaStatus
.ent iReferSemaStatus
iReferSemaStatus:
                        j       __syscall
                        addiu   v1, zero, -72
.end iReferSemaStatus

.global RFU073
.ent RFU073
RFU073:
                        j       __syscall
                        addiu   v1, zero, 73
.end RFU073

.global SetOsdConfigParam
.ent SetOsdConfigParam
SetOsdConfigParam:
                        j       __syscall
                        addiu   v1, zero, 74
.end SetOsdConfigParam

.global GetOsdConfigParam
.ent GetOsdConfigParam
GetOsdConfigParam:
                        j       __syscall
                        addiu   v1, zero, 75
.end GetOsdConfigParam

.global GetGsHParam
.ent GetGsHParam
GetGsHParam:
                        j       __syscall
                        addiu   v1, zero, 76
.end GetGsHParam

.global GetGsVParam
.ent GetGsVParam
GetGsVParam:
                        j       __syscall
                        addiu   v1, zero, 77
.end GetGsVParam

.global SetGsHParam
.ent SetGsHParam
SetGsHParam:
                        j       __syscall
                        addiu   v1, zero, 78
.end SetGsHParam

.global SetGsVParam
.ent SetGsVParam
SetGsVParam:
                        j       __syscall
                        addiu   v1, zero, 79
.end SetGsVParam

.global RFU080_CreateEventFlag
.ent RFU080_CreateEventFlag
RFU080_CreateEventFlag:
                        j       __syscall
                        addiu   v1, zero, 80
.end RFU080_CreateEventFlag

.global RFU081_DeleteEventFlag
.ent RFU081_DeleteEventFlag
RFU081_DeleteEventFlag:
                        j       __syscall
                        addiu   v1, zero, 81
.end RFU081_DeleteEventFlag

.global RFU082_SetEventFlag
.ent RFU082_SetEventFlag
RFU082_SetEventFlag:
                        j       __syscall
                        addiu   v1, zero, 82
.end RFU082_SetEventFlag

.global RFU083_iSetEventFlag
.ent RFU083_iSetEventFlag
RFU083_iSetEventFlag:
                        j       __syscall
                        addiu   v1, zero, 83
.end RFU083_iSetEventFlag

.global RFU084_ClearEventFlag
.ent RFU084_ClearEventFlag
RFU084_ClearEventFlag:
                        j       __syscall
                        addiu   v1, zero, 84
.end RFU084_ClearEventFlag

.global RFU085_iClearEventFlag
.ent RFU085_iClearEventFlag
RFU085_iClearEventFlag:
                        j       __syscall
                        addiu   v1, zero, 85
.end RFU085_iClearEventFlag

.global RFU086_WaitEvnetFlag
.ent RFU086_WaitEvnetFlag
RFU086_WaitEvnetFlag:
                        j       __syscall
                        addiu   v1, zero, 86
.end RFU086_WaitEvnetFlag

.global RFU087_PollEvnetFlag
.ent RFU087_PollEvnetFlag
RFU087_PollEvnetFlag:
                        j       __syscall
                        addiu   v1, zero, 87
.end RFU087_PollEvnetFlag

.global RFU088_iPollEvnetFlag
.ent RFU088_iPollEvnetFlag
RFU088_iPollEvnetFlag:
                        j       __syscall
                        addiu   v1, zero, 88
.end RFU088_iPollEvnetFlag

.global RFU089_ReferEventFlagStatus
.ent RFU089_ReferEventFlagStatus
RFU089_ReferEventFlagStatus:
                        j       __syscall
                        addiu   v1, zero, 89
.end RFU089_ReferEventFlagStatus

.global RFU090_iReferEventFlagStatus
.ent RFU090_iReferEventFlagStatus
RFU090_iReferEventFlagStatus:
                        j       __syscall
                        addiu   v1, zero, -90
.end RFU090_iReferEventFlagStatus

.global RFU091
.ent RFU091
RFU091:
                        j       __syscall
                        addiu   v1, zero, 91
.end RFU091

.global EnableIntcHandler
.ent EnableIntcHandler
EnableIntcHandler:
                        j       __syscall
                        addiu   v1, zero, 92
.end EnableIntcHandler

.global iEnableIntcHandler
.ent iEnableIntcHandler
iEnableIntcHandler:
                        j       __syscall
                        addiu   v1, zero, -92
.end iEnableIntcHandler

.global DisableIntcHandler
.ent DisableIntcHandler
DisableIntcHandler:
                        j       __syscall
                        addiu   v1, zero, 93
.end DisableIntcHandler

.global iDisableIntcHandler
.ent iDisableIntcHandler
iDisableIntcHandler:
                        j       __syscall
                        addiu   v1, zero, -93
.end iDisableIntcHandler

.global EnableDmacHandler
.ent EnableDmacHandler
EnableDmacHandler:
                        j       __syscall
                        addiu   v1, zero, 94
.end EnableDmacHandler

.global iEnableDmacHandler
.ent iEnableDmacHandler
iEnableDmacHandler:
                        j       __syscall
                        addiu   v1, zero, -94
.end iEnableDmacHandler

.global DisableDmacHandler
.ent DisableDmacHandler
DisableDmacHandler:
                        j       __syscall
                        addiu   v1, zero, 95
.end DisableDmacHandler

.global iDisableDmacHandler
.ent iDisableDmacHandler
iDisableDmacHandler:
                        j       __syscall
                        addiu   v1, zero, -95
.end iDisableDmacHandler

.global KSeg0
.ent KSeg0
KSeg0:
                        j       __syscall
                        addiu   v1, zero, 96
.end KSeg0

.global EnableCache
.ent EnableCache
EnableCache:
                        j       __syscall
                        addiu   v1, zero, 97
.end EnableCache

.global DisableCache
.ent DisableCache
DisableCache:
                        j       __syscall
                        addiu   v1, zero, 98
.end DisableCache

.global GetCop0
.ent GetCop0
GetCop0:
                        j       __syscall
                        addiu   v1, zero, 99
.end GetCop0

.global FlushCache
.ent FlushCache
FlushCache:
                        j       __syscall
                        addiu   v1, zero, 100
.end FlushCache

.global CpuConfig
.ent CpuConfig
CpuConfig:
                        j       __syscall
                        addiu   v1, zero, 102
.end CpuConfig

.global iGetCop0
.ent iGetCop0
iGetCop0:
                        j       __syscall
                        addiu   v1, zero, -103
.end iGetCop0

.global iFlushCache
.ent iFlushCache
iFlushCache:
                        j       __syscall
                        addiu   v1, zero, -104
.end iFlushCache

.global iCpuConfig
.ent iCpuConfig
iCpuConfig:
                        j       __syscall
                        addiu   v1, zero, -106
.end iCpuConfig

.global SifStopDma
.ent SifStopDma
SifStopDma:
                        j       __syscall
                        addiu   v1, zero, 107
.end SifStopDma

.global SetCPUTimerHandler
.ent SetCPUTimerHandler
SetCPUTimerHandler:
                        j       __syscall
                        addiu   v1, zero, 108
.end SetCPUTimerHandler

.global SetCPUTimer
.ent SetCPUTimer
SetCPUTimer:
                        j       __syscall
                        addiu   v1, zero, 109
.end SetCPUTimer

.global SetOsdConfigParam2
.ent SetOsdConfigParam2
SetOsdConfigParam2:
                        j       __syscall
                        addiu   v1, zero, 110
.end SetOsdConfigParam2

.global GetOsdConfigParam2
.ent GetOsdConfigParam2
GetOsdConfigParam2:
                        j       __syscall
                        addiu   v1, zero, 111
.end GetOsdConfigParam2

.global GsGetIMR
.ent GsGetIMR
GsGetIMR:
                        j       __syscall
                        addiu   v1, zero, 112
.end GsGetIMR

.global iGsGetIMR
.ent iGsGetIMR
iGsGetIMR:
                        j       __syscall
                        addiu   v1, zero, -112
.end iGsGetIMR

.global GsPutIMR
.ent GsPutIMR
GsPutIMR:
                        j       __syscall
                        addiu   v1, zero, 113
.end GsPutIMR

.global iGsPutIMR
.ent iGsPutIMR
iGsPutIMR:
                        j       __syscall
                        addiu   v1, zero, -113
.end iGsPutIMR

.global SetPgifHandler
.ent SetPgifHandler
SetPgifHandler:
                        j       __syscall
                        addiu   v1, zero, 114
.end SetPgifHandler

.global SetVSyncFlag
.ent SetVSyncFlag
SetVSyncFlag:
                        j       __syscall
                        addiu   v1, zero, 115
.end SetVSyncFlag

.global RFU116
.ent RFU116
RFU116:
                        j       __syscall
                        addiu   v1, zero, 116
.end RFU116

.global print
.ent print
print:
                        j       __syscall
                        addiu   v1, zero, 117
.end print

.global SifDmaStat
.ent SifDmaStat
SifDmaStat:
                        j       __syscall
                        addiu   v1, zero, 118
.end SifDmaStat

.global iSifDmaStat
.ent iSifDmaStat
iSifDmaStat:
                        j       __syscall
                        addiu   v1, zero, -118
.end iSifDmaStat


.global SifSetDma
.ent SifSetDma
SifSetDma:
                        j       __syscall
                        addiu   v1, zero, 119
.end SifSetDma

.global iSifSetDma
.ent iSifSetDma
iSifSetDma:
                        j       __syscall
                        addiu   v1, zero, -119
.end iSifSetDma


.global SifSetDChain
.ent SifSetDChain
SifSetDChain:
                        j       __syscall
                        addiu   v1, zero, 120
.end SifSetDChain

.global iSifSetDChain
.ent iSifSetDChain
iSifSetDChain:
                        j       __syscall
                        addiu   v1, zero, -120
.end iSifSetDChain


.global SifSetReg
.ent SifSetReg
SifSetReg:
                        j       __syscall
                        addiu   v1, zero, 121
.end SifSetReg


.global SifGetReg
.ent SifGetReg
SifGetReg:
                        j       __syscall
                        addiu   v1, zero, 122
.end SifGetReg


.global ExecOSD
.ent ExecOSD
ExecOSD:
                        j       __syscall
                        addiu   v1, zero, 123
.end ExecOSD

.global Deci2Call
.ent Deci2Call
Deci2Call:
                        j       __syscall
                        addiu   v1, zero, 124
.end Deci2Call

.global PSMode
.ent PSMode
PSMode:
                        j       __syscall
                        addiu   v1, zero, 125
.end PSMode

.global MachineType
.ent MachineType
MachineType:
                        j       __syscall
                        addiu   v1, zero, 126
.end MachineType


.global GetMemorySize
.ent GetMemorySize
GetMemorySize:
                        j       __syscall
                        addiu    v1, zero, 127
.end GetMemorySize

.global __syscall
.ent __syscall
__syscall:
                        mfc0        v0, $12
                        andi        v0, v0, 0x18
                        beqz        v0, _kMode
                        nop
                        syscall
                        jr          ra
                        nop
_kMode:
                        sll         v1, v1, 2
                        lui         k0, 0x8000
                        lhu         v0, 0x02F0(k0)
                        sll         v0, v0, 16
                        lh          k0, 0x02F8(k0)
                        add         v0, v0, k0
                        addu        v1, v1, v0
                        lw          k0, 0x00(v1)
                        jr          k0
                        nop
.end __syscall
