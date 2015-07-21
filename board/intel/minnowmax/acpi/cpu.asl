/* These devices are created at runtime */
External (\_PR.CP00, DeviceObj)
External (\_PR.CP01, DeviceObj)
External (\_PR.CP02, DeviceObj)
External (\_PR.CP03, DeviceObj)

/* Notify OS to re-read CPU tables, assuming ^2 CPU count */
Method (PNOT)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CP00, 0x81)  // _CST
		Notify (\_PR.CP01, 0x81)  // _CST
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CP02, 0x81)  // _CST
		Notify (\_PR.CP03, 0x81)  // _CST
	}
}

/* Notify OS to re-read CPU _PPC limit, assuming ^2 CPU count */
Method (PPCN)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CP00, 0x80)  // _PPC
		Notify (\_PR.CP01, 0x80)  // _PPC
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CP02, 0x80)  // _PPC
		Notify (\_PR.CP03, 0x80)  // _PPC
	}
}

/* Notify OS to re-read Throttle Limit tables, assuming ^2 CPU count */
Method (TNOT)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CP00, 0x82)  // _TPC
		Notify (\_PR.CP01, 0x82)  // _TPC
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CP02, 0x82)  // _TPC
		Notify (\_PR.CP03, 0x82)  // _TPC
	}
}

/* Return a package containing enabled processor entries */
Method (PPKG)
{
	If (LGreaterEqual (\PCNT, 4)) {
		Return (Package() {\_PR.CP00, \_PR.CP01, \_PR.CP02, \_PR.CP03})
	} ElseIf (LGreaterEqual (\PCNT, 2)) {
		Return (Package() {\_PR.CP00, \_PR.CP01})
	} Else {
		Return (Package() {\_PR.CP00})
	}
}
