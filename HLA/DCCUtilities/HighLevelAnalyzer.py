# High Level Analyzer
# For more information and documentation, please go to https://support.saleae.com/extensions/high-level-analyzer-extensions

from saleae.analyzers import HighLevelAnalyzer, AnalyzerFrame, StringSetting, NumberSetting, ChoicesSetting

from DCCPacket import DCCPacket
 
		
# High level analyzers must subclass the HighLevelAnalyzer class.
class Hla(HighLevelAnalyzer):

	Packet = DCCPacket()
	
	def __init__(self):
		return
	
	def get_capabilities(self):
		return
	
	def set_settings(self, settings):
		return {
			'result_types': {
				'error': {
					'format': 'Error!'
				},
				"packet": {
					'format': 'preamble; sbit; address: {{data.address}}; [ {{sbit}, {cmd.data}} ]; checksum; pebit'
				}
			}
		}
	
	def decode(self, frame: AnalyzerFrame):
		
		result = self.Packet.Decode(frame)
		if len(result) == 4:
			ptype    = result[0]
			pstime   = result[1]
			petime   = result[2]
			presult  = result[3]
			return AnalyzerFrame(ptype, pstime, petime, presult)
				
