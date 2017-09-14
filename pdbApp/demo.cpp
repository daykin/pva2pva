
#include <epicsMath.h>
#include <dbAccess.h>
#include <dbScan.h>
#include <recGbl.h>
#include <alarm.h>

#include <waveformRecord.h>
#include <menuFtype.h>

#include <epicsExport.h>

namespace {

// pi/180
static const double pi_180 = 0.017453292519943295;

int dummy;

long init_spin(waveformRecord *prec)
{
    if(prec->ftvl==menuFtypeDOUBLE)
        prec->dpvt = &dummy;
    return 0;
}

long process_spin(waveformRecord *prec)
{
    const double freq = 360.0*pi_180/100; // rad/sample
    double phase = 0;
    double *val = static_cast<double*>(prec->bptr);

    long ret = dbGetLink(&prec->inp, DBF_DOUBLE, &phase, 0, 0);
    if(ret) {
        (void)recGblSetSevr(prec, LINK_ALARM, INVALID_ALARM);
        return ret;
    }

    phase *= pi_180; // deg -> rad

    for(size_t i=0, N=prec->nelm; i<N; i++)
        val[i] = sin(freq*i+phase);

    prec->nord = prec->nelm;

    return 0;
}

template<typename REC>
struct dset5
{
    long count;
    long (*report)(int);
    long (*init)(int);
    long (*init_record)(REC *);
    long (*get_ioint_info)(int, REC *, IOSCANPVT*);
    long (*process)(REC *);
};

dset5<waveformRecord> devWfPDBDemo = {5,0,0,&init_spin,0,&process_spin};

} // namespace

extern "C" {
epicsExportAddress(dset, devWfPDBDemo);
}
