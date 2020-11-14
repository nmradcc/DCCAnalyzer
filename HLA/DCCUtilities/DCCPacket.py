
from saleae.analyzers import HighLevelAnalyzer, AnalyzerFrame, StringSetting, NumberSetting, ChoicesSetting


class DCCPacket:
	def __init__(self):
		self.DCC_BASELINE_PACKET_SPEED_OFFSET = 3
		
		self.State = None
		self.Type = ""
		self.PreambleBits = 0
		self.Address = 0
		self.Data = []
		self.Checksum = 0
		self.StartTime = None
		self.EndTime = None
		self.Result = { "data": "" }
		self.NextByte = 0

	def Reset(self):
		self.State = None
		self.Type = ""
		self.PreambleBits = 0
		self.Address = 0
		self.Data = []
		self.Checksum = 0
		self.StartTime = None
		self.EndTime = None
		self.Result = { "data": "" }
		self.NextByte = 0

	def Process(self, frame: AnalyzerFrame):
		self.EndTime = frame.end_time
		self.Type = 'Packet'
		result = self.parse_address()
		if self.NextByte <= (len(self.Data)-1):
			result += ", "
			result += self.parse_command()
		while (self.NextByte <= (len(self.Data)-1)):
			result += ", %02x" % self.Data[self.NextByte]
			self.NextByte += 1
			
		self.Result['data'] = result
		
	def Error(self, frame: AnalyzerFrame):
		self.EndTime = frame.end_time
		if self.StartTime == None:
			self.StartTime = frame.start_time
		self.Type = 'Error'
		result = "Error detected with %s" % self.State
		self.Result['data'] = result

	def Decode(self, frame: AnalyzerFrame):
		retval = []
		valid = False
		if (self.State == None):
			if frame.type == 'preamble':
				preamble_bits = frame.data['data'][0]
				print("Preamble %d bits" % preamble_bits)
				self.StartTime = frame.start_time
				self.PreambleBits = preamble_bits
				self.State = 'sbit-a'
				valid = True
		elif (self.State == 'sbit-a'):
			if frame.type == 'sbit':
				self.State = 'address'
				valid = True
		elif (self.State == 'address'):
			if frame.type == 'addr':
				address_byte = frame.data['data'][0]
				print("Address %x bits" % address_byte)
				self.Address = address_byte
				self.State = 'sbit-c'
				valid = True
		elif (self.State == 'sbit-c'):
			if frame.type == 'sbit':
				self.State = 'command'
				valid = True
		elif (self.State == 'command'):
			if frame.type == 'data':
				data_byte = frame.data['data'][0]
				print("Data %x bits" % data_byte)
				self.Data.append(data_byte)
				self.State = 'sbit-d' 
				valid = True
		elif (self.State == 'sbit-d'):
			if frame.type == 'sbit':
				self.State = 'data'
				valid = True
		elif (self.State == 'data'):
			if frame.type == 'data':
				data_byte = frame.data['data'][0]
				print("Data %x bits" % data_byte)
				self.Data.append(data_byte)
				self.State = 'sbit-d'
				valid = True
			elif (frame.type == 'checksum'):
				checksum_byte = frame.data['data'][0]
				print("Checksum %x bits" % checksum_byte)
				self.Checksum = checksum_byte
				self.State = 'end'
				valid = True
		elif (self.State == 'end'):
			if frame.type == 'pebit':
				self.Process(frame)
				ptype    = self.Type
				pstime   = self.StartTime
				petime   = self.EndTime
				presult  = self.Result
				self.Reset()
				retval = [ptype, pstime, petime, presult]
				valid = True

		if not valid:
			self.Error(frame)
			ptype    = self.Type
			pstime   = self.StartTime
			petime   = self.EndTime
			presult  = self.Result
			self.Reset()
			retval = [ptype, pstime, petime, presult]
					
		return retval

	def parse_address(self):
		
		if (self.Address == 0):
			return "broadcast address"
		elif (self.Address >0 and self.Address < 128):
			return "decoder short address=%d" % self.Address
		elif (self.Address > 127 and self.Address < 192):
			return "accessory address=%d" % self.Address
		elif (self.Address >191 and self.Address < 232):
			msaddr = self.Address & 0x3F
			lsaddr = self.Data[0]
			long_address = (msaddr << 8) | lsaddr
			self.NextByte = 1
			return "decoder long address=%d" % long_address
		elif (self.Address == 255):
			return "idle"
		else:
			return "RFU: 0x%x" % address_byte
		
	def parse_service_mode(self, data):
		cmd = ((data >> 2) & 0x03)
		val = data & 0x03
		if cmd == 0 or cmd == 1:
			return "Verify AH:%x" % val
		elif cmd == 2:
			return "Bits AH:%x" % val
		else:
			return "Write AH:%x" % val

	def parse_accessory(self, data):
		if (data & 0x80):
			addr = (~(data >> 4)) & 0x07
			out = (data & 0x07)
			state = (data & 0x08)
			if state:
				return "AddrH %d Out %d ON" % (addr, out)
			else:
				return "AddrH %d Out %d OFF" % (addr, out)
		else:
			addr = data ^ 0x70
			return "AddrH %0x" % addr

	def parse_command(self):
		cmd_msb = self.Data[self.NextByte] >> 4
		cmd_lsb = self.Data[self.NextByte] & 0x0F
		if cmd_msb == 0:
			if cmd_lsb == 0:
				retval = "Reset"
			elif cmd_lsb == 1:
				retval = "Hard Reset"
			elif cmd_lsb == 2 or cmd_lsb == 3:
				retval = "Factory Test"
			elif cmd_lsb == 6 or cmd_lsb == 7:
				retval = "Set Flags"
			elif cmd_lsb == 10 or cmd_lsb == 11:
				retval = "Set Adv Adr"
			elif cmd_lsb == 15:
				retval = "Req Ack"
			else:
				retval = "Reserved"
		elif cmd_msb == 1:
			if cmd_lsb == 2:
				retval = "Set Consist FWD"
			elif cmd_lsb == 3:
				retval = "Set Consist REV"
			else:
				retval = "Reserved"
		elif cmd_msb == 3:
			if cmd_lsb == 13:
				retval = "Analog Function"
			elif cmd_lsb == 14:
				retval = "Restricted Speed"
			elif cmd_lsb == 15:
				self.NextByte += 1
				st128dir = self.Data[self.NextByte] & 0x80
				st128spd = self.Data[self.NextByte] & 0x7f
				if (st128dir == 0x80):
					dirstr = "FWD"
				else:
					dirstr = "REV"
				if (st128spd == 0):
					retval = "Speed 128 %s STOP" % dirstr
				elif (st128spd == 1):
					retval = "Speed 128 %s ESTOP" % dirstr
				else:
					retval = "Speed 128 %s %d" % (dirstr, (st128spd-1))
			else:
				retval = "Reserved"
		elif cmd_msb == 4 or cmd_msb == 5:
			cSpeed = (cmd_lsb << 1) | (cmd_msb & 0x01)
			if cmd_lsb == 0 or cmd_lsb == 1:
				retval = "Speed 14/28 REV STOP"
			elif cmd_lsb == 2 or cmd_lsb == 3:
				retval = "Speed 14/28 REV ESTOP"
			else:
				retval = "Speed 14/28 REV %d" % (cSpeed - self.DCC_BASELINE_PACKET_SPEED_OFFSET)
		elif cmd_msb == 6 or cmd_msb == 7:
			cSpeed = (cmd_lsb << 1) | (cmd_msb & 0x01)
			if cmd_lsb == 0 or cmd_lsb == 1:
				retval = "Speed FWD STOP"
			elif cmd_lsb == 2 or cmd_lsb == 3:
				retval = "Speed FWD ESTOP"
			else:
				retval = "Speed FWD %d" % (cSpeed - self.DCC_BASELINE_PACKET_SPEED_OFFSET)
		elif cmd_msb == 8 or cmd_msb == 9:
			if (self.Data[self.NextByte] & 0x10):
				retval = "Func grp 1 ON %d" % cmd_lsb
			else:
				retval = "Func grp 1 OFF %d" % cmd_lsb
		elif cmd_msb == 10 or cmd_msb == 11:
			if (self.Data[self.NextByte] & 0x10):
				retval = "Func grp 2 L %d" % cmd_lsb
			else:
				retval = "Func grp 2 H %d" % cmd_lsb
		elif cmd_msb == 12 or cmd_msb == 13:
			state = self.Data[self.NextByte] & 0x1F
			if state == 0:
				retval = "Binary State Long"
			elif state == 0x1D:
				retval = "Binary State Short"
			elif state == 0x1E:
				retval = "F13-F20 Control"
			elif state == 0x1F:
				retval = "F21-F28 Control"
			else:
				retval = "Reserved"
		elif cmd_msb == 14:
			state = ((self.Data[self.NextByte] >> 2) & 0x03)
			val = self.Data[self.NextByte] & 0x03
			if state == 0:
				retval = "CV Long Reserved"
			elif state == 1:
				retval = "CV Long Verify %x" % val
			elif state == 2:
				retval = "CV Long BITS %x" % val
			elif state == 3:
				retval = "CV Long Write %x" % val
			else:
				retval = "Reserved"
		elif cmd_msb == 15:
			if cmd_lsb == 0:
				retval = "CV Short N/A"
			elif cmd_lsb == 2:
				retval = "CV Short Accelerate"
			elif cmd_lsb == 3:
				retval = "CV Short Decelerate"
			elif cmd_lsb == 9:
				retval = "Decoder Lock"
			else:
				retval = "CV Short Reserved"
		else:
			 retval = "Reserved"
			 
		self.NextByte += 1	 
		return retval

