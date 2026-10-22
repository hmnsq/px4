
#సూచన once

#అన్తర్భావ <inttypes.h>
#అన్తర్భావ <string.h>

#నిర్వచించు ATOMIC_ENTER తాళం()
#నిర్వచించు ATOMIC_LEAVE unlock()

నామాకాశ cdev
{

నిర్మాణం file_operations {
	శూన్య *op;
};

ఉపయోగించి px4_file_operations_t = నిర్మాణం file_operations;
ఉపయోగించి mode_t = uint32_t;

తరగతి CDev;

నిర్మాణం file_t {
	పూర్ణాంకం f_oflags{0};
	శూన్య *f_priv{శూన్యసూచిక};
	CDev *cdev{శూన్యసూచిక};

	file_t() = అప్రమేయ;
	file_t(పూర్ణాంకం f, CDev *c) : f_oflags(f), cdev(c) {}
};

} // namespace cdev

బాహ్య "C" __EXPORT పూర్ణాంకం రిజిస్టరు_driver(స్థిరరాశి అక్షరం *name, స్థిరరాశి cdev::px4_file_operations_t *fops,
					cdev::mode_t బహుళకం, శూన్య *దత్తాంశం);
బాహ్య "C" __EXPORT పూర్ణాంకం unregister_driver(స్థిరరాశి అక్షరం *మార్గం);
